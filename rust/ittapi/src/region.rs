use std::ffi::CString;

/// An Intel® Processor Trace region.
pub struct Region(ittapi_sys::__itt_pt_region);
impl Region {
    /// Create a new Intel PT region.
    ///
    /// ```
    /// # use ittapi::Region;
    /// let region = Region::new("test-region");
    /// ```
    ///
    /// # Panics
    ///
    /// Panics if the domain name contains a `0` byte.
    #[must_use]
    pub fn new(name: &str) -> Self {
        let c_string =
            CString::new(name).expect("unable to create a CString; does it contain a 0 byte?");
        #[cfg(unix)]
        let create_fn = unsafe { ittapi_sys::__itt_pt_region_create_ptr__3_0 };
        #[cfg(windows)]
        let create_fn = unsafe { ittapi_sys::__itt_pt_region_createA_ptr__3_0 };
        let region = if let Some(create_fn) = create_fn {
            unsafe { create_fn(c_string.as_ptr()) }
        } else {
            // Use this value as a sentinel to indicate that the region was not created.
            u8::MAX
        };
        Self(region)
    }

    /// Mark a section of code as an Intel PT region using `__itt_mark_pt_region_begin`. This can be
    /// used for fine-grained profiling, such as [anomaly detection] (a preview feature of VTune).
    ///
    /// [anomaly detection]:
    ///     https://www.intel.com/content/www/us/en/docs/vtune-profiler/user-guide/current/anomaly-detection-analysis.html
    ///
    /// ```
    /// # use ittapi::Region;
    /// let region = Region::new("test-region");
    /// // Mark a region for fine-grained measurement, such as a tight loop.
    /// for _ in 0..10 {
    ///     let _marked = region.mark();
    ///     let _ = 2 + 2;
    ///     // Marked region ends here, when dropped; use `end()` to end it explicitly.
    /// }
    /// ```
    #[inline]
    #[must_use]
    pub fn mark(&self) -> MarkedRegion {
        unsafe { ittapi_sys::__itt_mark_pt_region_begin(self.0) };
        MarkedRegion(self)
    }
}

/// A [`MarkedRegion`] is a Rust helper structure for ergonomically ending a marked region using
/// `__itt_mark_pt_region_end`. See [`Region::mark`] for more details.
pub struct MarkedRegion<'a>(&'a Region);
impl MarkedRegion<'_> {
    /// End the marked region.
    #[inline]
    pub fn end(self) {
        // Do nothing; the `Drop` implementation does the work. See discussion at
        // https://stackoverflow.com/questions/53254645.
    }
}
impl Drop for MarkedRegion<'_> {
    #[inline]
    fn drop(&mut self) {
        unsafe { ittapi_sys::__itt_mark_pt_region_end(self.0 .0) };
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    #[should_panic(expected = "unable to create a CString; does it contain a 0 byte?")]
    fn zero_byte() {
        let _region = Region::new("zero\0byte\0name");
    }

    #[test]
    fn sanity() {
        let region = Region::new("region");
        for _ in 0..10 {
            let _marked_region = region.mark();
            // Do nothing.
            _marked_region.end();
        }
    }
}
