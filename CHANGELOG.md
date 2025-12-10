# Changelog

## [Unreleased]

### Added

* Prevented cmake to add rpaths into binaries
* Prevented cmake to add full paths into binaries
* Prevented build system to add debug directories in windows build
* Made binaries stripped except macOs arm64
* On macOS with IDA, use Qt6 for headers but don't link - plugin will link IDA's Qt

### Fixed

* config.json turns a folder instead of a file. now it stays as a json file
* if macOS arm64 is stripped then IDA cannot load plugin. Error message is
  `MH_DYLIB is missing LC_ID_DYLIB`. On macOS ARM64 (Apple Silicon), the
  dynamic linker (dyld) is much stricter about Mach-O header consistency
  than it is on `x86_64`.

## [2025-09-14]

### Added

* Support for IDA Pro v9.2 and Qt6

### Removed

* IDA Pro v9.1 support removed
* Qt5 support removed
* Created a new branch for IDA pro v9.1 and Qt5

## [2024-10-29][2024-10-29]

### Added

* IDA Pro v9.0 support
* Prebuilt binaries provided

[unreleased]: https://github.com/blue-devil/ifred/compare/v2025.09.14...HEAD
[2025-09-14]: https://github.com/blue-devil/ifred/compare/v2024.10.29...v2025.09.14
[2024-10-29]: https://github.com/blue-devil/ifred/releases/tag/v2024.10.29
