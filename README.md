Intel® Instrumentation and Tracing Technology (ITT) and Just-In-Time (JIT) APIs
===============================================================================

[![Build Status](https://github.com/intel/ittapi/actions/workflows/main.yml/badge.svg?branch=master&event=push)](https://github.com/intel/ittapi/actions)
[![CodeQL](https://github.com/intel/ittapi/actions/workflows/codeql.yml/badge.svg?branch=master)](https://github.com/intel/ittapi/security/code-scanning/tools/CodeQL/status)
[![Package on conda-forge](https://img.shields.io/conda/vn/conda-forge/ittapi.svg)](https://anaconda.org/conda-forge/ittapi)
[![Package on PyPI](https://img.shields.io/pypi/v/ittapi)](https://pypi.org/project/ittapi)
[![Package on crates.io](https://img.shields.io/crates/v/ittapi.svg)](https://crates.io/crates/ittapi)
[![OpenSSF Scorecard](https://api.securityscorecards.dev/projects/github.com/intel/ittapi/badge)](https://securityscorecards.dev/viewer/?uri=github.com/intel/ittapi)

This repository contains the following profiling APIs:

- **Instrumentation and Tracing Technology (ITT) API**:
  Powers your application to generate and control the collection of trace data
  during its execution, seamlessly integrating with Intel tools.
-  **Just-In-Time (JIT) Profiling API**:
  Reports detailed information about just-in-time (JIT) compiled code, enabling
  you to profile the performance of dynamically generated code.

The ITT/JIT APIs consist of two parts:

- **Static Part**: An open-source static library that you compile and link with your application.
- **Dynamic Part**: A tool-specific shared library that collects and writes trace data. You can
  find the reference implementation of the dynamic part as a *Reference Collector*
  [here](./src/ittnotify_refcol/README.md).

### Build

To build the library:

- Get general development tools, including C/C++ Compiler
- Install [Python](https://python.org) 3.6 or later
- Install [CMake](https://cmake.org) 3.5 or later
- For a Windows* system, install one of these:
  - [Microsoft Visual Studio](https://visualstudio.microsoft.com) 2015 or later
  - [Ninja](https://github.com/ninja-build/ninja/releases) 1.9 or later
-  To enable support for Fortran, install the
  [Intel Fortran Compiler](https://www.intel.com/content/www/us/en/docs/fortran-compiler/get-started-guide/current/overview.html)
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

### Documentation

Find complete documentation for ITT/JIT APIs on the
[ITT/JIT APIs Documentation Page](https://intel.github.io/ittapi)

### License

All code in the repo is dual licensed under GPLv2 and 3-Clause BSD licenses

### Make Contributions

Learn how to contribute using our [contribution guide](CONTRIBUTING.md)

To report bugs or request enhancements, please use the [Issues page on GitHub](https://github.com/intel/ittapi/issues)

### Security

To report vulnerabilities, refer to our [security policy](SECURITY.md)
