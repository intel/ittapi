# ittapi Integration Tests

This crate checks the functionality of the high-level `ittapi` crate using a VTune installation on
the current system. It exists solely for testing in a specially configured environment and will not
be published.

### Test

On a system with VTune installed:

```
source vtune-vars.sh
cargo test
```

To install VTune, see the [User Guide]. The environment setup script (e.g., `vtune-vars.sh`) can be
found within the VTune installation; e.g., `$HOME/intel/oneapi/vtune/latest` or
`/opt/intel/oneapi/vtune/latest` on [Linux].

[User Guide]: https://www.intel.com/content/www/us/en/docs/vtune-profiler/user-guide/current/installation.html
[Linux]: https://www.intel.com/content/www/us/en/docs/vtune-profiler/get-started-guide/current/linux-os.html
