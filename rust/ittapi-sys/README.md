# ittapi-sys

[![Build Status](https://github.com/intel/ittapi/workflows/CI/badge.svg)][ci]
[![Documentation Status](https://docs.rs/ittapi-sys/badge.svg)][docs]

This crate contains low-level Rust bindings for the C `ittapi` library--you likely want to use the
[high-level Rust crate]. The `ittapi` library is used for various aspects of Intel&reg; profiling;
it exposes the Instrumentation and Tracing Technology (ITT) API as well as the Just-In-Time (JIT)
Profiling API. More details about `ittapi` are available on its [README].

[ci]: https://github.com/intel/ittapi/actions/workflows/main.yml
[docs]: https://docs.rs/ittapi-sys
[high-level Rust crate]: https://crates.io/crates/ittapi
[README]: https://github.com/intel/ittapi#readme

> IMPORTANT NOTE: this crate is currently only tested on Linux, macOS, and Windows platforms but
> support for other platforms is intended; contributions are welcome!

If you are interested in using VTune to profile Rust applications, you may find the following guide
helpful: [Wasmtime Docs: Using VTune on
Linux](https://docs.wasmtime.dev/examples-profiling-vtune.html)


### Use

```toml
[dependencies]
ittapi-sys = "0.3"
```


### Build

```
cargo build
```

Building `ittapi-sys` will build the `ittapi` C library and link it statically into your
application; see the [build.rs] file.

[build.rs]: https://github.com/intel/ittapi/blob/master/rust/build.rs

_For Windows developers_: this crate uses a symbolic link to access the C library it depends on. To
modify this crate on Windows, either [configure Git to understand POSIX symlinks] or use the
[copy-c-library.ps1] script to temporarily copy the files.

[configure Git to understand POSIX symlinks]: https://github.com/git-for-windows/git/wiki/Symbolic-Links
[copy-c-library.ps1]: scripts/copy-c-library.ps1


### Test

```sh
cargo test
```

This crate's tests ensure the `ittapi-sys` bindings remain up to date with the [official C header
files]; they do not check `ittapi` functionality.

[official C header files]: https://github.com/intel/ittapi/tree/master/include


### Regenerate Bindings

If the `ittapi-sys` bindings are not up to date, they can be regenerated with:

```sh
BLESS=1 cargo test
```

The binding generation uses `bindgen`. An in-depth description of how to use `bindgen` is available
in [the `bindgen` documentation][bindgen docs].

[bindgen docs]: https://rust-lang.github.io/rust-bindgen/
