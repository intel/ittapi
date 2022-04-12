//! The JIT (Just-In-Time) Profiling API provides functionality to report information about
//! just-in-time generated code that can be used by performance tools. The [Jit] Rust structure is a
//! high-level view of a subset of the full functionality available. See the [JIT Profiling API] for
//! more information.
//!
//! [JIT Profiling API]:
///     https://www.intel.com/content/www/us/en/develop/documentation/vtune-help/top/api-support/jit-profiling-api.html
use anyhow::Context;
use std::{ffi::CString, os, ptr};

/// Register JIT-compiled methods with a performance tool (e.g., VTune). This structure assumes
/// single-threaded access; if your program may be multi-threaded, make sure to guard multi-threaded
/// access with a mutex.
#[derive(Default)]
pub struct Jit {
    shutdown_complete: bool,
}

impl Jit {
    /// Returns a new `MethodId` for use in `MethodLoad` events.
    #[allow(clippy::unused_self)]
    pub fn get_method_id(&self) -> MethodId {
        MethodId(unsafe { ittapi_sys::iJIT_GetNewMethodID() })
    }

    /// Notifies any `EventType` to VTune.
    ///
    /// # Errors
    ///
    /// May fail if the underlying call to the ITT library fails.
    #[allow(clippy::unused_self)]
    pub fn notify_event(&self, mut event: EventType) -> anyhow::Result<()> {
        let tag = event.tag();
        let data = event.data();
        log::trace!("notify_event: tag={:?}", tag);
        let res = unsafe { ittapi_sys::iJIT_NotifyEvent(tag, data) };
        if res == 1 {
            Ok(())
        } else {
            anyhow::bail!("error when notifying event")
        }
    }

    // High-level helpers.

    /// Notifies VTune that a new function described by the `MethodLoadBuilder` has been jitted.
    ///
    /// # Errors
    ///
    /// May fail if the builder has invalid data or if the ITT library fails to notify the method
    /// load event.
    pub fn load_method(&self, builder: MethodLoadBuilder) -> anyhow::Result<()> {
        let method_id = self.get_method_id();
        let method_load = builder.build(method_id)?;
        self.notify_event(method_load)
    }

    /// Notifies VTune that profiling is being shut down.
    ///
    /// # Errors
    ///
    /// May fail if the ITT library fails to notify the shutdown event.
    pub fn shutdown(&mut self) -> anyhow::Result<()> {
        let res = self.notify_event(EventType::Shutdown);
        if res.is_ok() {
            self.shutdown_complete = true;
        }
        res
    }
}

impl Drop for Jit {
    fn drop(&mut self) {
        if !self.shutdown_complete {
            // There's not much we can do when an error happens here.
            if let Err(err) = self.shutdown() {
                log::error!("Error when shutting down VTune: {}", err);
            }
        }
    }
}

/// Type of event to be dispatched through ittapi's JIT event API.
pub enum EventType {
    /// Send this event after a JITted method has been loaded into memory, and possibly JIT
    /// compiled, but before the code is executed.
    MethodLoadFinished(MethodLoad),

    /// Send this notification to terminate profiling.
    Shutdown,
}

impl EventType {
    /// Returns the C event type to be used when notifying this event to VTune.
    fn tag(&self) -> ittapi_sys::iJIT_jvm_event {
        match self {
            EventType::MethodLoadFinished(_) => {
                ittapi_sys::iJIT_jvm_event_iJVM_EVENT_TYPE_METHOD_INLINE_LOAD_FINISHED
            }
            EventType::Shutdown => ittapi_sys::iJIT_jvm_event_iJVM_EVENT_TYPE_SHUTDOWN,
        }
    }

    /// Returns a raw C pointer to the event-specific data that must be used when notifying this
    /// event to VTune.
    fn data(&mut self) -> *mut os::raw::c_void {
        match self {
            EventType::MethodLoadFinished(method_load) => ptr::addr_of_mut!(method_load.0).cast(),
            EventType::Shutdown => ptr::null_mut(),
        }
    }
}

/// Newtype wrapper for a method id returned by ittapi's `iJIT_GetNewMethodID`, as returned by
/// `VtuneState::get_method_id` in the high-level API.
#[derive(Clone, Copy)]
pub struct MethodId(u32);

/// Newtype wrapper for a JIT method load.
pub struct MethodLoad(ittapi_sys::_iJIT_Method_Load);

/// Multi-step constructor using the builder pattern for a `MethodLoad` event.
pub struct MethodLoadBuilder {
    method_name: String,
    addr: *const u8,
    len: usize,
    class_file_name: Option<String>,
    source_file_name: Option<String>,
}

impl MethodLoadBuilder {
    /// Creates a new `MethodLoadBuilder` from scratch.
    ///
    /// `addr` is the pointer to the start of the code region, `len` is the size of this code
    /// region in bytes.
    pub fn new(method_name: String, addr: *const u8, len: usize) -> Self {
        Self {
            method_name,
            addr,
            len,
            class_file_name: None,
            source_file_name: None,
        }
    }

    /// Attache a class file.
    pub fn class_file_name(mut self, class_file_name: String) -> Self {
        self.class_file_name = Some(class_file_name);
        self
    }

    /// Attach a source file.
    pub fn source_file_name(mut self, source_file_name: String) -> Self {
        self.source_file_name = Some(source_file_name);
        self
    }

    /// Build a "method load" event for the given `method_id`.
    ///
    /// # Errors
    ///
    /// May fail if the various names passed to this builder are not valid C strings.
    pub fn build(self, method_id: MethodId) -> anyhow::Result<EventType> {
        Ok(EventType::MethodLoadFinished(MethodLoad(
            ittapi_sys::_iJIT_Method_Load {
                method_id: method_id.0,
                method_name: CString::new(self.method_name)
                    .context("CString::new failed")?
                    .into_raw(),
                method_load_address: self.addr as *mut os::raw::c_void,
                method_size: self.len.try_into().expect("cannot fit length into 32 bits"),
                line_number_size: 0,
                line_number_table: ptr::null_mut(),
                class_id: 0, // Field officially obsolete in Intel's doc.
                class_file_name: CString::new(
                    self.class_file_name
                        .as_deref()
                        .unwrap_or("<unknown class file name>"),
                )
                .context("CString::new failed")?
                .into_raw(),
                source_file_name: CString::new(
                    self.source_file_name
                        .as_deref()
                        .unwrap_or("<unknown source file name>"),
                )
                .context("CString::new failed")?
                .into_raw(),
            },
        )))
    }
}
