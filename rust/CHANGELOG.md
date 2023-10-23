# Changelog
All notable changes to this project will be documented in this file.

## [0.4.0] - 2023-10-23
### Fixed
- Fixed a bug where JIT-generated code was not visible to VTune
  ([#105](https://github.com/intel/ittapi/pull/105))

## [0.3.5] - 2023-10-05
### Changed
- Removed some Fortran object files ([#100](https://github.com/intel/ittapi/pull/100))
- Regenerated the bindings, updated dependencies ([#102](https://github.com/intel/ittapi/pull/102))

## [0.3.4] - 2023-07-28
### Added
- OpenBSD support added
### Changed
- Fixed double shutdown in JIT API ([#82](https://github.com/intel/ittapi/issues/82))

## [0.3.3] - 2023-01-18
### Added
- FreeBSD support; the crate now contains bindings for Windows, Linux, macOS, and FreeBSD

## [0.3.2] - 2022-11-14
### Added
- Add `pause`, `resume`, and `detach` functions

## [0.3.1] - 2022-04-12
### Changed
- Make high-level `Domain` structure `Sync`

## [0.3.0] - 2022-04-06
### Changed
- Split the functionality into two crates: `ittapi-sys` for the low-level C bindings and `ittapi`
  for new high-level APIs (e.g., Domain, Task, etc.)

## [0.2.0] - 2022-02-16
### Added
- Windows support; the crate now contains bindings for Windows, Linux, and macOS
### Removed
- The `force32` feature; 32-bit binaries are unsupported until someone requests to re-add this

## [0.1.6] - 2021-12-17
### Changed
- Fixed the license string in `Cargo.toml`
- Improved documentation
