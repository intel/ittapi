use std::{ffi::CString, marker::PhantomData};

/// See the [Event API] documentation.
///
/// [Event API]: https://www.intel.com/content/www/us/en/develop/documentation/vtune-help/top/api-support/instrumentation-and-tracing-technology-apis/instrumentation-tracing-technology-api-reference/event-api.html
pub struct Event(ittapi_sys::__itt_event);
impl Event {
    /// Create the event.
    pub fn new(name: &str) -> Self {
        #[cfg(unix)]
        let create_fn = unsafe { ittapi_sys::__itt_event_create_ptr__3_0 };
        #[cfg(windows)]
        let create_fn = unsafe { ittapi_sys::__itt_event_createA_ptr__3_0 };
        if let Some(create_fn) = create_fn {
            let c_string =
                CString::new(name).expect("unable to create a CString; does it contain a 0 byte?");
            let size = name
                .len()
                .try_into()
                .expect("unable to fit the name length into an i32");
            let event = unsafe { create_fn(c_string.as_ptr(), size) };
            Self(event)
        } else {
            Self(-1)
        }
    }

    /// Start the event.
    ///
    /// # Panics
    ///
    /// This will panic if the ITT library cannot start the event.
    pub fn start(&self) -> StartedEvent {
        if let Some(start_fn) = unsafe { ittapi_sys::__itt_event_start_ptr__3_0 } {
            let result = unsafe { start_fn(self.0) };
            assert!(result == 0, "unable to start event");
        }
        StartedEvent {
            event: self.0,
            phantom: PhantomData,
        }
    }
}

pub struct StartedEvent<'a> {
    event: ittapi_sys::__itt_event,
    phantom: PhantomData<&'a mut ()>,
}

impl StartedEvent<'_> {
    /// End the event.
    #[allow(clippy::unused_self)]
    pub fn end(self) {
        // Do nothing; the `Drop` implementation does the work. See discussion at
        // https://stackoverflow.com/questions/53254645.
    }
}

impl<'a> Drop for StartedEvent<'a> {
    fn drop(&mut self) {
        if let Some(end_fn) = unsafe { ittapi_sys::__itt_event_end_ptr__3_0 } {
            let result = unsafe { end_fn(self.event) };
            assert!(result == 0, "unable to stop event");
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn sanity() {
        let event = Event::new("test");
        let _started_event = event.start();
        // Dropping `started_event` ends the event.
    }
}
