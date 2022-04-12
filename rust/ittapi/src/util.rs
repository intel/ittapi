/// Provide a convenient way to access ittapi functions that may have not been initialized. The
/// `ittnotify` library has a static part (e.g., `libittnotify.a`) and a dynamic part (e.g.,
/// `libittnotify_collector.so`, VTune Profiler). The static part provides the ITT symbols to the
/// application but these may not be resolved to actual implementations in the dynamic part--the
/// data collector.
macro_rules! access_sys_fn {
    ($fn_name: ident) => {
        unsafe {
            ittapi_sys::$fn_name.expect(concat!(
                "unable to access the ittapi function: ",
                stringify!($fn_name)
            ))
        }
    };
}

// This allows the macro to be used internally without being published.
pub(crate) use access_sys_fn;
