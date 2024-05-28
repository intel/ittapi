use crate::util::access_sys_fn;
use std::ffi::CString;

/// String handles are ITT's mechanism for efficiently naming objects. See the [String Handle API]
/// documentation for more information.
///
/// [String Handle API]:
///     https://www.intel.com/content/www/us/en/docs/vtune-profiler/user-guide/current/string-handle-api.html
#[derive(PartialEq, Eq, Debug)]
pub struct StringHandle(*mut ittapi_sys::__itt_string_handle);
impl StringHandle {
    /// Create a new string handle; repeated calls with the same `name` will return the same handle.
    ///
    /// ```
    /// # use ittapi::StringHandle;
    /// let a = StringHandle::new("test");
    /// let b: StringHandle = "test".into();
    /// assert_eq!(a, b);
    /// ```
    ///
    /// # Panics
    ///
    /// Panics if the string contains a `0` byte.
    #[must_use]
    pub fn new(name: &str) -> Self {
        #[cfg(unix)]
        let create_fn = access_sys_fn!(__itt_string_handle_create_ptr__3_0);
        #[cfg(windows)]
        let create_fn = access_sys_fn!(__itt_string_handle_createA_ptr__3_0);
        let c_string =
            CString::new(name).expect("unable to create a CString; does it contain a 0 byte?");
        let handle = unsafe { create_fn(c_string.as_ptr()) };
        Self(handle)
    }

    /// Use the `__itt_string_handle` pointer internally.
    pub(crate) fn as_ptr(&self) -> *mut ittapi_sys::__itt_string_handle {
        self.0
    }
}

impl From<&str> for StringHandle {
    fn from(name: &str) -> Self {
        Self::new(name)
    }
}

mod tests {
    #[test]
    fn construct_string_handle() {
        let sh = super::StringHandle::new("test2");
        // `ittapi` profiling is only enabled when the `INTEL_LIBITTNOTIFY64` environment variable
        // is set, which points to the profiling collector. If set, the string handle is actually
        // allocated; if not set, no allocation is performed (this is a general `ittapi` property:
        // no allocation without a profiling collector). Note that string handles are thus invalid
        // in the latter case--no collector--but the API safely does not provide any way to observe
        // this.
        if std::env::var("INTEL_LIBITTNOTIFY64").is_ok() {
            assert!(!sh.as_ptr().is_null());
        } else {
            assert!(sh.as_ptr().is_null());
        }
    }
}
