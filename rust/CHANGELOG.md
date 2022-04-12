# Changelog
All notable changes to this project will be documented in this file.

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
