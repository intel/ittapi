# ittapi-rs

This package contains Rust bindings for the `ittapi` library--it is equivalent to a crate using [the
`*-sys` convention][convention] but named differently for historical reasons. The `ittapi` library
is used for various aspects of Intel&reg; profiling; it exposes the Instrumentation and Tracing
Technology (ITT) API as well as the Just-In-Time (JIT) Profiling API. More details about `ittapi`
are available on its [README].

[README]: https://github.com/intel/ittapi#readme
[convention]: https://doc.rust-lang.org/cargo/reference/build-scripts.html#-sys-packages

> IMPORTANT NOTE: this package is currently only tested on several Linux platforms (recent Ubuntu
> and Fedora builds) but support for other platforms is intended; contributions are welcome!

If you are interested in using VTune to profile Rust applications, you may find the following guide
helpful: [Wasmtime Docs: Using VTune on
Linux](https://docs.wasmtime.dev/examples-profiling-vtune.html)


### Use

```toml
[dependencies]
ittapi = "0.1"
```

This crate currently uses the parent project's CMake configuration so a local CMake installation (as
well as a C compiler) is necessary.


### Build

```
cargo build
```

Building `ittapi-rs` will use CMake to build the `ittapi` library and link it statically into your
application; see the [build.rs] file.

[build.rs]: https://github.com/intel/ittapi/blob/master/ittapi-rs/build.rs


### Test

```sh
cargo test
```

This crate's tests ensure the `ittapi-rs` bindings remain up to date with the [official C header
files]; they do not check `ittapi` functionality.

[official C header files]: https://github.com/intel/ittapi/tree/master/include


### Regenerate Bindings

If the `ittapi-rs` bindings are not up to date, they can be regenerated with:

```sh
BLESS=1 cargo test
```

The binding generation uses `bindgen`. An in-depth description of how to use `bindgen` is available
in [the `bindgen` documentation][bindgen docs].

[bindgen docs]: https://rust-lang.github.io/rust-bindgen/
