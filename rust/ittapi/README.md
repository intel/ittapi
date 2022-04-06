# ittapi

[![Build Status](https://github.com/intel/ittapi/workflows/CI/badge.svg)][ci]
[![Documentation Status](https://docs.rs/ittapi/badge.svg)][docs]

This crate allows Rust programs to use Intel&reg; Instrumentation and Tracing Technology (ITT) APIs.
Currently, the following APIs are supported (please submit an issue or PR for additional support):
 - a JIT notification API, a higher-level view of the C [JIT Profiling API]
 - the Domain API
 - the Task API
 - the String Handle API
 - the Event API

This uses the [`ittapi-sys`] crate which depends on the [C `ittapi` library].

[ci]: https://github.com/intel/ittapi/actions/workflows/main.yml
[docs]: https://docs.rs/ittapi
[JIT Profiling API]: https://www.intel.com/content/www/us/en/develop/documentation/vtune-help/top/api-support/jit-profiling-api.html
[`ittapi-sys`]: https://github.com/intel/ittapi/tree/master/rust/ittapi-sys
[C `ittapi` library]: https://github.com/intel/ittapi

> IMPORTANT NOTE: this crate is currently only tested on Linux, macOS, and Windows platforms but
> support for other platforms is intended; contributions are welcome!

If you are interested in using VTune to profile Rust applications, you may find the following guide
helpful: [Wasmtime Docs: Using VTune on
Linux](https://docs.wasmtime.dev/examples-profiling-vtune.html)


### Use

```toml
[dependencies]
ittapi = "0.3"
```

### Build

```
cargo build
```

### Test

```sh
cargo test
```
