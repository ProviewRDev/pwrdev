# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- Added triage bot to the project.
- Added issue templates.
- Added some ABB classes to the workbench palette for quick access.
- Added the cJSON library to the project.
- Added this changelog.

### Fixed

- Deb archive installation script would cause the script to fail since the service file had been renewed.
- Templates for objects inheriting PnDevice are now fixed.
- Animation bug in javascript ge graphs caused execution to stop in the browser.
- Fix build scripts where some directories where not created as they should when using parallel build.
- The "Alarm and event analyzer" would not start due to missing environmental variables.
- Fix a bug that caused the operator graph to crash when an input lost focus.
- Some aggregates for ABB and Eurotherm did not have their dynamic graphs generated correctly. They do now.
- Fix some renamed/misspelled images for the documentation.
- Fix the SsEnd/SsBegin objects for grafcet. They could not be used with boolean inputs and if connected prior to an upgrade they generated errorenous code.
- QCom had a high chance of deadlocking the entire QCom library on a node due to incorrect signal handling, causing communication outage.
  - Signal handling is removed in QCom leaving it up to the user to handle application specifics in regards to signals, as it should be.
  - Signal handling was added to the most common utilities used such as rt_xtt and rt_rtt where signals will be handled in an application specific manner and exit accordingly without deadlocks.
- DsFastCurve could crash because of an uninitialized variable used for the TriggIndex. It is now fixed.
- The Event Log would not convert Event Name to the correct coding resulting in garbled text in the Event Name and no results in the search.
- PROFINET: The configurator was unprepared for unpopulated slot 0. It is now well prepared for this.
- PROFINET: The configurator could show some inconsistencies in slots when changing DAP when the starting index of the physical slots did not start at 0.
- PROFINET: The configurator will now truncate strings to 79 characters before writing attributes using cdh-functions that doesn't add null termination.

### Changed

- The rs_export_rtdb executable now uses the common library for cJSON.
- Simplify the dir_symbols.mk file.
- $pwrp_doc is now created on rt systems.
- pwr_stop.sh has been updated to remove the POSIX log message queue.
- profibus: The PROFINET configurator was not prepared for multiple fixed Virtual Submodule Items, it is now.
- profibus: PROFINET startup sequence now waits StartupTime seconds before checking IOCS status from devices
- Updated the documentation for the PID controller.
- PROFINET: The configurator will now rename modules and change the description if you change them. For instance, when changing DAP.
- PROFINET: The configurator will now do a sanity check on the modules configured for the DAP and remove submodules not in the submodules list for the DAP
  - This could happen during an upgrade when inconsistency in the GSDML files used as source would imply that a submodule should exists. This is due to the way things were implemented in the old configurator using simple indexes to map DAPs.

### Removed

- Java support, only the websocket server will remain java for now. This was however removed several versions back. It's here to make it official.

[unreleased]: https://gitlab.ssab.com/pwrdev/pwr/-/compare/V6.1.3...HEAD
