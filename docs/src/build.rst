.. _build:

Build from Source Code
======================

Technical Requirements
----------------------

Before you build the ITT/JIT APIs, make sure you have the following hardware and software tools:

-  Get general development tools, including C/C++ Compiler
-  Install `Python <https://python.org>`__  3.6 or later
-  Install `CMake <https://cmake.org>`__ 3.5 or later
-  For a Windows* system, install one of these:
   
   -  `Microsoft Visual Studio <https://visualstudio.microsoft.com>`__ 2015 or later
   -  `Ninja <https://ninja-build.org>`__ 1.9 or later.

-  To enable support for Fortran, install the `Intel Fortran Compiler
   <https://www.intel.com/content/www/us/en/developer/tools/oneapi/fortran-compiler-download.html>`__


Get the ITT/JIT APIs Source Code
--------------------------------

To get the source code for the ITT/JIT APIs, do one of the following:

-  Download it from `the latest public Release <https://github.com/intel/ittapi/releases>`__
-  Clone the repository:

.. code-block:: console

	git clone https://github.com/intel/ittapi.git


Build the ITT/JIT APIs
----------------------

To build the ITT/JIT APIs static library, run this command:

.. code-block:: console

	python buildall.py <options>


Use these options to configure the build process:

.. code-block:: console

    usage: python buildall.py [-h] [-d] [-c] [-v] [-pt] [-ft] [--force_bits]

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
