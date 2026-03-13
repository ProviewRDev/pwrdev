# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [V7.0.0] - 2026-04-01

### Added

- Added triage bot to the project.
- Added issue templates.
- Added some ABB classes to the workbench palette for quick access.
- Added the cJSON library to the project.
- Added this changelog.
- Add an error section to the bottom of the help window that displays up to 20 error messages.
  - Improve contextual feedback in the help window (e.g., via XTT, WTT) with clearer, more actionable error messages.
  - Include detailed error information, such as the filename and line number where each issue occurs.
- Added BitField8/16/32/64 data representation. These represent bitfields that are not byte-swapped, in accordance with their definition.
- New pin for DSup, ActiveNotAcked for when the DSup is active but has not yet been ackknowledged.
- A new Profinet Viewer has been added with more functionality and a topology view.
- CoLogger has been added (co_logger.h) that can be used in applications for easy logging in RFC5424 format.
- bcomp: Added MinOut to compposit
- PROFIBUS: Added support for jokerblocks introduced in GSD revision 4/5
- Accessibility support (ATK interface implementations) for widgets created by GE editor. Enabling external onscreen touch keyboards to popup when focusing input fields, if installed and activated.
- RemnodeUDP: Added PMTU discovery setting (Do/Don't Fragment, DF bit).
- A new pwr_upgrade tool has been added that unifies what reload.sh and upgrade.sh did. It should simplify the upgrade procedure for both users and devs.
  - reload.sh and upgrade.sh will be kept until the next version
- Added packet-qcom.lua plugin for Wireshark. This can be copied to the Wireshark lua plugin folder which one can find in the wireshark menu Help->About Wireshark->Folders (tab).
  - Start Wireshark from a terminal with PWR_BUS_ID set and it will find and dissect qcom messages with the filter "qcom".
- Support for treating char arrays as strings in ge using format '%s' on a datatype like ##Char#16.
- PROFINET: IM0 tab in object graph for class PnDevice.
- PROFINET: Added a configurable Startup Mode choice under Device Properties in the configurator, with guidance for when Legacy may be preferable if Advanced startup causes issues with large fragmented startup traffic.

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
- Fix error handling for help-related issues, ensuring that "file not found" and "topic not found" errors are properly managed.
- DsFastCurve could crash because of an uninitialized variable used for the TriggIndex. It is now fixed.
- The Event Log would not convert Event Name to the correct coding resulting in garbled text in the Event Name and no results in the search.
- PROFINET: The configurator was unprepared for unpopulated slot 0. It is now well prepared for this.
- PROFINET: The configurator could show some inconsistencies in slots when changing DAP when the starting index of the physical slots did not start at 0.
- PROFINET: The configurator will now truncate strings to 79 characters before writing attributes using cdh-functions that doesn't add null termination.
- PROFIBUS: The configurator has more known keywords, reducing amount of console ouptut.
- PROFIBUS: The configurator supports multilines where windows style CRLF where used reducing weird console output that could occur.
- PID + CompPID: Fixed incorrect function when PidAlg was set to only I. It will now integrate correct with respect to Inverse flag.
- Traces for ConstAv and ConstIv are now working as they should. And can be used as one would expect.
- Fixed a display regression bug from color theme implementation in the workbench for various types across the workbench for void and status types. Code for DataArithms for instance, would show up in the DataIn1/2/3/4 attributes. And also, in objects where statuses are used, the value of the Status attribute would show up as the attribute value from the attribute above.
- Fixed a display bug in ge tables where buffers had no room for a null termination and thus data from the next cell was shown in the cell before.
  - Also added support for 64-bit integers and float, both size wise and with regards to formatting flags.
- Fixed rabbitmq + mqtt message size calculations
- UDP IO: Implemented stall action
- Various dereferenced null pointers in the workbench that caused the workbench to crash
- Message handler application requests no longer block for up to one hour waiting for a `qcom_Request()` reply. They now use the normal MH send/receive timeout and preserve `MH__TMO` on timeout.
- Distribution is now SSH-only. `RemoteAccessType` only offers `SSH`, and the old upgrade path rewrites older distribution settings to the SSH defaults.
- Package distribution now writes a per-target SSH log file and reports that path on failure.
- Closing a subwindow without adding anything to it or building it will no longer end up in an inconsistent state when trying to open that subwindow again.


### Changed

- The rs_export_rtdb executable now uses the common library for cJSON.
- Simplify the dir_symbols.mk file.
- $pwrp_doc is now created on rt systems and ready for distribution of documentation.
- pwr_stop.sh has been updated to remove the POSIX log message queue.
- profibus: The PROFINET configurator was not prepared for multiple fixed Virtual Submodule Items, it is now.
- profibus: PROFINET startup sequence now waits StartupTime seconds before checking IOCS status from devices
- Updated the documentation for the PID controller.
- PROFINET: The configurator will now rename modules and change the description if you change them. For instance, when changing DAP.
- PROFINET: The configurator will now do a sanity check on the modules configured for the DAP and remove submodules not in the submodules list for the DAP
  - This could happen during an upgrade when inconsistency in the GSDML files used as source would imply that a submodule should exists. This is due to the way things were implemented in the old configurator using simple indexes to map DAPs.
- Threads checking in on CycleSup objects will now tell you which CycleSup object triggered the EmergencyBreak.
- UDP (rs_remote_udpip) have had its buffer increased to 65507 bytes. But do try to keep UDP packets small.
- rt_errh/rt_errl has been changed to log in rfc5424 format.
- Xtt console log has been updated to read both old and new format of the proviewr log file (pwr_{hostname}.log).
- Xtt console log has had its filters and search improved and controls moved to the toolbar instead of a separate window.
- PROFINET: Softing headers have moved outside the codebase.
- Update Classes will now be more thorough and check sizes of class definitions compared to database and update accordingly. (https://gitlab.ssab.com/pwrdev/pwr/-/issues/189)
- Various exit/return paths changed in some wb tooling used in reload/upgrade process. Some errors have been downgraded to warnings which is more in line with how one would expect things to be.

### Deprecated
- Nodave for communcating with legacy S7 300/400 will be removed next release.
- MQ (dmq) which is a 32-bit message queue will be removed next release.

### Removed

- Java support, only the websocket server will remain java for now. This was however removed several versions back. It's here to make it official.
- Lucida Sans font due to it being licensed. All uses of this font will be replaced with Helvetica.
- The old Profinet Viewer has been removed and replaced with the new and improved one.
- Removed rt_version.dat. The runtime version is now compiled in from pwr_version.h instead of read from a file that was only created during packaging. This also fixes the empty Version field in the runtime status monitor on non-packaged (dev) builds.
- Removed all remnants of the Motif framework that was still present.

[unreleased]: https://gitlab.ssab.com/pwrdev/pwr/-/compare/V7.0.0...HEAD
[V7.0.0]: https://gitlab.ssab.com/pwrdev/pwr/-/compare/V6.1.3...V7.0.0
