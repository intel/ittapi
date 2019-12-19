# ittapi-rs
This package creates Rust bindings for the ittapi for instrumentation and tracking and just-in-time profiling.<br/>
<br/>
Note the building of this package is currently only supported (tested) on a couple of Linux platforms (recent Ubuntu and Fedora builds) but support for other platforms is intended and contributions are welcomed.<br/>

# Build the package
cargo build<br/>
<br/>
** Note this building package uses bindgen which in turn requires recent versions of cmake and llvm to be installed. Specifically for Fedora it was "yum install llvm-devel" was needed to bring in llvm-config.<br/>
<br/>
** Also note building this package requires rust nightly.<br/>

# Publish the package to crates.io<br/>
cargo publish<br/>
