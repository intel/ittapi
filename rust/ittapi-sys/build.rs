//! Build the `ittapi` C library in the parent directory. The `cc` configuration here should match
//! that of the parent directories `CMakeLists.txt` (TODO: keep these in sync, see [#36]).
//!
//! [#36]: https://github.com/intel/ittapi/issues/36

fn main() {
    let mut build = cc::Build::new();
    build
        .file("c-library/src/ittnotify/ittnotify_static.c")
        .file("c-library/src/ittnotify/jitprofiling.c")
        .include("c-library/src/ittnotify/")
        .include("c-library/include/");

    // For `x86_64-pc-windows-gnu` targets (i.e., MinGW builds), the `strnlen_s` function may not
    // always be available.
    if let Ok("x86_64-pc-windows-gnu") = std::env::var("TARGET").as_deref() {
        build
            .flag("--enable-secure-api")
            .define("MINGW_HAS_SECURE_API", "1")
            .define("SDL_STRNLEN_S", None);
    }

    build.compile("ittnotify");
}
