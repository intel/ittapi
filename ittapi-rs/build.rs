//! Build the `ittapi` C library in the parent directory. The `cc` configuration here should match
//! that of the parent directories `CMakeLists.txt` (TODO: keep these in sync,
//! https://github.com/intel/ittapi/issues/36).
#![allow(unused)]
use std::env;
use std::path::PathBuf;

fn main() {
    let out_dir = env::var("OUT_DIR").unwrap();
    let out_path = PathBuf::from(out_dir);

    #[cfg(not(feature = "force_32"))]
    {
        cc::Build::new()
            .file("ittnotify/ittnotify_static.c")
            .file("ittnotify/jitprofiling.c")
            .include("ittnotify/")
            .include("include/")
            .compile("ittnotify");
    }

    #[cfg(feature = "force_32")]
    #[cfg(not(any(target_os = "ios", target_os = "macos")))]
    {
        cc::Build::new()
            .file("ittnotify/ittnotify_static.c")
            .file("ittnotify/jitprofiling.c")
            .define("FORCE_32", "ON")
            .include("ittnotify/")
            .include("include/")
            .compile("ittnotify");
    }
}
