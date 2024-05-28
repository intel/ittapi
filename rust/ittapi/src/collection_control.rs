/// Run the application without collecting data.
///
/// VTune Profiler reduces the overhead of collection, by collecting only
/// critical information, such as thread and process creation.
///
/// # Effects
///
/// Pausing the data collection has the following effects:
///
/// - Data collection is paused for the whole program, not only within the current thread.
/// - Some runtime analysis overhead reduction.
///
/// The following APIs are *not* affected by pausing the data collection:
///
/// - Domain API
/// - String Handle API
/// - Thread Naming API
///
/// The following APIs are affected by pausing the data collection. Data is not
/// collected for these APIs while in paused state:
///
/// - Task API
/// - Frame API
/// - Event API
/// - User-Defined Synchronization API
///
/// See the [Collection Control API] for more details.
///
/// # Example
///
/// ```
/// ittapi::pause();
/// // Do initialization work here
/// ittapi::resume();
/// // Do profiling work here
/// ittapi::pause();
/// // Do finalization work here
/// ```
///
/// [Collection Control API]: https://www.intel.com/content/www/us/en/docs/vtune-profiler/user-guide/current/collection-control-api.html
pub fn pause() {
    if let Some(pause_fn) = unsafe { ittapi_sys::__itt_pause_ptr__3_0 } {
        unsafe { (pause_fn)() };
    }
}

/// Resume data collection.
///
/// VTune Profiler resumes collecting all data.
///
/// See the [Collection Control API] for details.
///
/// # Example
///
/// See [pause#example].
///
/// [Collection Control API]: https://www.intel.com/content/www/us/en/docs/vtune-profiler/user-guide/current/collection-control-api.html
pub fn resume() {
    if let Some(resume_fn) = unsafe { ittapi_sys::__itt_resume_ptr__3_0 } {
        unsafe { (resume_fn)() };
    }
}

/// Detach data collection.
///
/// VTune Profiler detaches all collectors from all processes. Your application
/// continues to work but no data is collected for the running collection.
///
/// See the [Collection Control API] for details.
///
/// # Example
///
/// Detach behaves similarly to [pause#example].
///
/// [Collection Control API]: https://www.intel.com/content/www/us/en/docs/vtune-profiler/user-guide/current/collection-control-api.html
pub fn detach() {
    if let Some(detach_fn) = unsafe { ittapi_sys::__itt_detach_ptr__3_0 } {
        unsafe { (detach_fn)() };
    }
}
