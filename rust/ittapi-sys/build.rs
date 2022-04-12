//! Build the `ittapi` C library in the parent directory. The `cc` configuration here should match
//! that of the parent directories `CMakeLists.txt` (TODO: keep these in sync, see [#36]).
//!
//! [#36]: https://github.com/intel/ittapi/issues/36

fn main() {
    cc::Build::new()
        .file("c-library/src/ittnotify/ittnotify_static.c")
        .file("c-library/src/ittnotify/jitprofiling.c")
        .include("c-library/src/ittnotify/")
        .include("c-library/include/")
        .compile("ittnotify");
}
