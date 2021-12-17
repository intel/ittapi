#![allow(unused)]
use std::env;
use std::path::PathBuf;

#[cfg(target_os = "windows")]
fn main() {}

#[cfg(not(target_os = "windows"))]
fn main() {
    let out_dir = env::var("OUT_DIR").unwrap();
    let out_path = PathBuf::from(out_dir);

    #[cfg(not(feature = "force_32"))]
    {
        cc::Build::new().file("src/ittnotify/ittnotify_static.c")
            .file("src/ittnotify/jitprofiling.c")
            .include("src/ittnotify/")
            .include("include/")
            .compile("ittnotify64");
    }

    #[cfg(feature = "force_32")]
    #[cfg(not(any(target_os = "ios", target_os = "macos")))]
    {
        cc::Build::new().file("src/ittnotify/ittnotify_static.c")
            .file("src/ittnotify/jitprofiling.c")
            .define("FORCE_32", "ON")
            .include("src/ittnotify/")
            .include("include/")
            .compile("ittnotify32");
    }
}
