use anyhow::Context;
use std::{
    ffi::CString,
    sync::{Mutex, MutexGuard},
};

pub struct RecordMethodBuilder {
    method_name: String,
    addr: *const u8,
    len: usize,

    class_file_name: Option<String>,
    source_file_name: Option<String>,
}

impl RecordMethodBuilder {
    pub fn new(method_name: String, addr: *const u8, len: usize) -> Self {
        Self {
            method_name,
            addr,
            len,
            class_file_name: None,
            source_file_name: None,
        }
    }
    pub fn class_file_name(mut self, class_file_name: String) -> Self {
        self.class_file_name = Some(class_file_name);
        self
    }
    pub fn source_file_name(mut self, source_file_name: String) -> Self {
        self.source_file_name = Some(source_file_name);
        self
    }
    pub fn build(self, state: &InnerVtuneState) -> anyhow::Result<()> {
        state.record_method(self)
    }
}

pub struct InnerVtuneState;

pub struct VtuneState {
    inner: Mutex<InnerVtuneState>,
}

impl VtuneState {
    pub fn get(&mut self) -> MutexGuard<InnerVtuneState> {
        self.inner.lock().unwrap()
    }
}

impl Drop for VtuneState {
    fn drop(&mut self) {
        self.inner.lock().unwrap().shutdown();
    }
}

impl InnerVtuneState {
    fn record_method(&self, builder: RecordMethodBuilder) -> anyhow::Result<()> {
        let method_id = get_method_id();

        let mut jmethod = ittapi_sys::_iJIT_Method_Load {
            method_id,
            method_name: CString::new(builder.method_name)
                .context("CString::new failed")?
                .into_raw(),
            method_load_address: builder.addr as *mut ::std::os::raw::c_void,
            method_size: builder.len as u32,
            line_number_size: 0,
            line_number_table: std::ptr::null_mut(),
            class_id: 0,
            class_file_name: CString::new(
                builder
                    .class_file_name
                    .as_ref()
                    .map(|s| s.as_str())
                    .unwrap_or("<unknown class file name>"),
            )
            .context("CString::new failed")?
            .into_raw(),
            source_file_name: CString::new(
                builder
                    .source_file_name
                    .as_ref()
                    .map(|s| s.as_str())
                    .unwrap_or("<unknown source file name>"),
            )
            .context("CString::new failed")?
            .into_raw(),
        };

        let jmethod_ptr = &mut jmethod as *mut _ as *mut _;
        unsafe {
            log::trace!("loaded new method (single method with id {})", method_id);
            let _ret = ittapi_sys::iJIT_NotifyEvent(
                ittapi_sys::iJIT_jvm_event_iJVM_EVENT_TYPE_METHOD_LOAD_FINISHED,
                jmethod_ptr as *mut ::std::os::raw::c_void,
            );
        }

        Ok(())
    }

    fn shutdown(&mut self) {
        log::trace!("NotifyEvent shutdown (whole module)");
        unsafe {
            let _ret = ittapi_sys::iJIT_NotifyEvent(
                ittapi_sys::iJIT_jvm_event_iJVM_EVENT_TYPE_SHUTDOWN,
                std::ptr::null_mut(),
            );
        }
    }
}

fn get_method_id() -> u32 {
    unsafe { ittapi_sys::iJIT_GetNewMethodID() }
}
