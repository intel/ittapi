// This is a smoke test for pre-generated `src/ittapi-bindings.rs` and
// `src/jitprofiling-bindings.rs` files to see that they don't need to be
// updated. We check in a generated version so downstream consumers don't
// have to get `bindgen` working themselves.
//
// If bindgen or ittapi.h or jitprofiling.h change you can run tests with
// `BLESS=1` (inspired by a similar patch for binaryen) to regenerate the
// source files, otherwise this can test on CI that the file doesn't need
// to be regenerated.

#![allow(non_snake_case, non_camel_case_types, non_upper_case_globals)]
#![allow(unused)]

const INCLUDE_PATH: &'static str = "./c-library/include";

#[cfg(target_os = "linux")]
const BINDINGS_PATH: &'static str = "src/linux";
#[cfg(target_os = "macos")]
const BINDINGS_PATH: &'static str = "src/macos";
#[cfg(target_os = "windows")]
const BINDINGS_PATH: &'static str = "src/windows";
#[cfg(target_os = "freebsd")]
const BINDINGS_PATH: &'static str = "src/freebsd";
#[cfg(target_os = "openbsd")]
const BINDINGS_PATH: &'static str = "src/openbsd";

#[test]
fn test_ittnotify_bindings_up_to_date() {
    // When generating the `ittnotify`, we exclude non-ITT constants (see `allowlist_var`) to avoid
    // `libc` differences.
    let mut expected = bindgen::Builder::default()
        .formatter(bindgen::Formatter::Rustfmt)
        .allowlist_var("ITT.*")
        .allowlist_var("__itt.*")
        // Also, note how few functions we allow: if we generate bindings for all the declared
        // functions, we run into linking errors. Only some functions are actually defined in
        // `libittnotify.a` but most are provided dynamically by the dynamic collection library. See
        // the `README.md` for more details.
        .allowlist_function("__itt_mark_pt.*")
        .header(concat(INCLUDE_PATH, "/ittnotify.h"))
        .generate()
        .expect("Unable to generate ittnotify bindings.")
        .to_string();

    let bindings_file = concat(BINDINGS_PATH, "/ittnotify_bindings.rs");
    if std::env::var("BLESS").is_ok() {
        std::fs::write(bindings_file, expected).unwrap();
    } else {
        let expected = normalize(expected);
        let actual = normalize(std::fs::read_to_string(bindings_file).unwrap());

        if expected == actual {
            return;
        }

        for diff in diff::lines(&expected, &actual) {
            match diff {
                diff::Result::Both(_, s) => println!(" {}", s),
                diff::Result::Left(s) => println!("-{}", s),
                diff::Result::Right(s) => println!("+{}", s),
            }
        }
        panic!("differences found, need to regenerate ittnotify bindings (`BLESS=1 cargo test`)");
    }
}

#[test]
fn test_jitprofiling_bindings_up_to_date() {
    let mut expected = bindgen::Builder::default()
        .formatter(bindgen::Formatter::Rustfmt)
        .header(concat(INCLUDE_PATH, "/jitprofiling.h"))
        .generate()
        .expect("Unable to generate jitprofiling bindings")
        .to_string();

    let bindings_file = concat(BINDINGS_PATH, "/jitprofiling_bindings.rs");
    if std::env::var("BLESS").is_ok() {
        std::fs::write(bindings_file, expected).unwrap();
    } else {
        let expected = normalize(expected);
        let actual = normalize(std::fs::read_to_string(bindings_file).unwrap());

        if expected == actual {
            return;
        }

        for diff in diff::lines(&expected, &actual) {
            match diff {
                diff::Result::Both(_, s) => println!(" {}", s),
                diff::Result::Left(s) => println!("-{}", s),
                diff::Result::Right(s) => println!("+{}", s),
            }
        }
        panic!(
            "differences found, need to regenerate jitprofiling bindings (`BLESS=1 cargo test`)"
        );
    }
}

/// Concatenate two strings; `concat!` only works with literals.
fn concat(a: &str, b: &str) -> String {
    format!("{}{}", a, b)
}

/// Normalize the line endings of a string; this removes Windows' carriage returns.
fn normalize(str: String) -> String {
    if cfg!(target_os = "windows") {
        str.replace("\r", "")
    } else {
        str
    }
}
