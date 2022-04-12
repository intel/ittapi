use crate::{domain::Domain, string::StringHandle};

/// A task is a logical unit of work performed by a particular thread. See the [Task API]
/// documentation for more information.
///
/// [Task API]:
///     https://www.intel.com/content/www/us/en/develop/documentation/vtune-help/top/api-support/instrumentation-and-tracing-technology-apis/instrumentation-tracing-technology-api-reference/task-api.html
///
/// ```
/// # use ittapi::{Domain, StringHandle, Task};
/// let _env_path = scoped_env::ScopedEnv::set("INTEL_LIBITTNOTIFY64", "<some path>");
/// let domain = Domain::new("domain");
/// let name = StringHandle::new("task");
/// // Measure a task using a pre-initialized string handle (most efficient).
/// {
///     let task = Task::begin(&domain, name);
///     let _ = 2 + 2;
///     task.end(); // Task ended here.
///     let _ = 3 + 3;
/// }
/// // Measure a task using a string reference (most convenient).
/// {
///     let _task = Task::begin(&domain, "task");
///     let _ = 2 + 2;
///     // Task ended here, when dropped.
/// }
/// ```
pub struct Task<'a>(&'a Domain);
impl<'a> Task<'a> {
    /// Start a task on the current thread.
    pub fn begin(domain: &'a Domain, name: impl Into<StringHandle>) -> Self {
        if let Some(create_fn) = unsafe { ittapi_sys::__itt_task_begin_ptr__3_0 } {
            // Currently the `taskid` and `parentid` parameters are unimplemented (TODO).
            unsafe { create_fn(domain.as_ptr(), ITT_NULL, ITT_NULL, name.into().as_ptr()) };
        }
        Self(domain)
    }

    /// Finish the task.
    #[allow(clippy::unused_self)]
    pub fn end(self) {
        // Do nothing; the `Drop` implementation does the work. See discussion at
        // https://stackoverflow.com/questions/53254645.
    }
}

impl<'a> Drop for Task<'a> {
    fn drop(&mut self) {
        // If `ittnotify` has not been initialized, this function may not be wired up.
        if let Some(end_fn) = unsafe { ittapi_sys::__itt_task_end_ptr__3_0 } {
            unsafe { end_fn(self.0.as_ptr()) }
        }
    }
}

/// Using the `__itt_null` symbol results in errors so we redefine it here.
const ITT_NULL: ittapi_sys::__itt_id = ittapi_sys::__itt_id {
    d1: 0,
    d2: 0,
    d3: 0,
};

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn sanity() {
        let domain = Domain::new("domain");
        let name = StringHandle::new("task");
        let _task = Task::begin(&domain, name);
    }
}
