//! This library allows Rust programs to use Intel&reg; Instrumentation and Tracing Technology (ITT)
//! APIs. These APIs are declared by a static library, [`ittnotify`], and dynamically used by
//! performance collection tools (e.g., `libittnotify_collector.so`, VTune Profiler).
//!
//! [`ittnotify`]: https://github.com/intel/ittapi
#![deny(missing_docs)]
#![deny(clippy::all)]
#![warn(clippy::pedantic)]
#![warn(clippy::cargo)]
#![allow(
    clippy::return_self_not_must_use,
    clippy::must_use_candidate,
    clippy::module_name_repetitions
)]

mod domain;
mod event;
pub mod jit;
mod string;
mod task;
mod util;

pub use domain::Domain;
pub use event::Event;
pub use string::StringHandle;
pub use task::Task;
