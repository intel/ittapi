Intel® Instrumentation and Tracing Technology (ITT) and Just-In-Time (JIT) API
==================================================================================

[![Build Status](https://github.com/intel/ittapi/actions/workflows/main.yml/badge.svg?branch=master&event=push)](https://github.com/intel/ittapi/actions)
[![CodeQL](https://github.com/intel/ittapi/actions/workflows/codeql.yml/badge.svg?branch=master)](https://github.com/intel/ittapi/security/code-scanning/tools/CodeQL/status)
[![Package on conda-forge](https://img.shields.io/conda/vn/conda-forge/ittapi.svg)](https://anaconda.org/conda-forge/ittapi)
[![Package on PyPI](https://img.shields.io/pypi/v/ittapi)](https://pypi.org/project/ittapi)
[![Package on crates.io](https://img.shields.io/crates/v/ittapi.svg)](https://crates.io/crates/ittapi)
[![OpenSSF Scorecard](https://api.securityscorecards.dev/projects/github.com/intel/ittapi/badge)](https://securityscorecards.dev/viewer/?uri=github.com/intel/ittapi)

This ITT/JIT open source profiling API includes:

  - Instrumentation and Tracing Technology (ITT) API
  - Just-In-Time (JIT) Profiling API

The Instrumentation and Tracing Technology (ITT) API enables your application
to generate and control the collection of trace data during its execution 
across different Intel tools.

ITT API consists of two parts: a _static part_ and a _dynamic part_. The
_dynamic part_ is specific for a tool and distributed only with a particular
tool. The _static part_ is a common part shared between tools. Currently, the
static part of ITT API is distributed as a static library and released under
a BSD/GPLv2 dual license with every tool supporting ITT API.

### Build

To build the library:
 - On Windows, Linux, FreeBSD and OSX: requires [cmake](https://cmake.org) to be set in `PATH`
 - Windows: requires Visual Studio installed or requires [Ninja](https://github.com/ninja-build/ninja/releases) to be set in `PATH`
 - To enable fortran support requires [Intel Fortran Compiler](https://www.intel.com/content/www/us/en/docs/fortran-compiler/get-started-guide/current/overview.html) installed
 - To list available build options execute: `python buildall.py -h`
```
usage: buildall.py [-h] [-d] [-c] [-v] [-pt] [-ft] [--force_bits]

optional arguments:
  -h, --help      show this help message and exit
  -d, --debug     specify debug build configuration (release by default)
  -c, --clean     delete any intermediate and output files
  -v, --verbose   enable verbose output from build process
  -pt, --ptmark   enable anomaly detection support
  -ft, --fortran  enable fortran support
  --force_bits    specify bit version for the target
  --vs            specify visual studio version (Windows only)
  --cmake_gen     specify cmake build generator (Windows only)
```
### License

All code in the repo is dual licensed under GPLv2 and 3-Clause BSD licenses

### Contributing

To contribute, please see our [contributing guide](CONTRIBUTING.md)  
To report bugs or request enhancements, please use the "Issues" page on GitHub

### Security

Please refer to the [security policy](SECURITY.md) for reporting vulnerabilties.
