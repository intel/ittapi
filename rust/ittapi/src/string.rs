use crate::util::access_sys_fn;
use std::ffi::CString;

/// String handles are ITT's mechanism for efficiently naming objects. See the [String Handle API]
/// documentation for more information.
///
/// [String Handle API]:
///     https://www.intel.com/content/www/us/en/develop/documentation/vtune-help/top/api-support/instrumentation-and-tracing-technology-apis/instrumentation-tracing-technology-api-reference/string-handle-api.html
#[derive(PartialEq, Debug)]
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
    fn with_dynamic_part_specified() {
        // When INTEL_LIBITTNOTIFY64 is set in the environment, the static part of ittnotify will
        // allocate; otherwise, if the dynamic part is not present, the pointer will be null.
        // let _env_path = scoped_env::ScopedEnv::set("INTEL_LIBITTNOTIFY64", "<some path>");
        let sh = super::StringHandle::new("test2");
        assert!(sh.as_ptr().is_null());
    }
}
