.. _build:

Build from Source
=================

Requirements for Building from Source
-------------------------------------

-  General development tools, including C/C++ compiler.
-  `Python <https://python.org>`__  3.6 or later.
-  `CMake <https://cmake.org>`__ 3.5 or later.
-  Windows only:
   `Microsoft Visual Studio <https://visualstudio.microsoft.com>`__ 2015 or later
   ether
   `Ninja <https://ninja-build.org>`__ 1.9 or later.
-  To enable Fortran support, requires `Intel Fortran Compiler
   <https://www.intel.com/content/www/us/en/developer/tools/oneapi/fortran-compiler-download.html>`__
   installed.


Download the Source Code
------------------------

Download `ITT/JIT API source code <https://github.com/intel/ittapi/releases>`__ or clone
`the repository <https://github.com/intel/ittapi.git>`__.

.. code-block:: console

	git clone https://github.com/intel/ittapi.git


Build
-----

To build the ITT/JIT API static library execute:

.. code-block:: console

	python buildall.py <options>


The ailable build options:

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
