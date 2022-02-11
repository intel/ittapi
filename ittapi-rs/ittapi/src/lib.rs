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
    pub fn build(self, state: &InnerVTuneState) -> anyhow::Result<()> {
        state.record_method(self)
    }
}

#[derive(Default)]
pub struct InnerVTuneState;

#[derive(Default)]
pub struct VTuneState {
    inner: Mutex<InnerVTuneState>,
}

impl VTuneState {
    pub fn new() -> Self {
        Default::default()
    }
    pub fn get(&self) -> MutexGuard<InnerVTuneState> {
        self.inner.lock().unwrap()
    }
}

impl Drop for VTuneState {
    fn drop(&mut self) {
        self.inner.lock().unwrap().shutdown();
    }
}

impl InnerVTuneState {
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
        log::trace!("loaded new method (single method with id {})", method_id);
        let res = unsafe {
            ittapi_sys::iJIT_NotifyEvent(
                ittapi_sys::iJIT_jvm_event_iJVM_EVENT_TYPE_METHOD_LOAD_FINISHED,
                jmethod_ptr as *mut ::std::os::raw::c_void,
            )
        };

        if res == 1 {
            Ok(())
        } else {
            anyhow::bail!("error when registering a method")
        }
    }

    fn shutdown(&mut self) {
        log::trace!("notify vtune about shutdown");
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
