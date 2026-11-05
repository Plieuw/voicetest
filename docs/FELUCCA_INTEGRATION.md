# Original Felucca integration baseline

## Rule

Development starts from the user's original Felucca installation. VoiceTest remains a separate repository and must not modify `fm1pokoe` or the upstream Felucca repository.

Upstream references:
- Felucca source: https://github.com/hugelton/Felucca
- Felucca build guide: https://github.com/hugelton/Felucca/blob/main/BUILDING.md
- Felucca installer: https://hugelton.github.io/Felucca/webapp/installer/

## What Felucca already provides

The upstream build guide describes a firmware app, an update loader and a `.fwsc` package. Its installer communicates with the FM-1 over USB-MIDI SysEx, lets the device read the package in two stages, waits for the loader/reboot and verifies the resulting device identity. The installer also supports returning to the official, unmodified M-VAVE V15 package after checking its SHA-256 digest.

This is a useful, proven design reference. It is not yet a drop-in VoiceTest installer:
- the upstream command-line installer deliberately accepts Felucca packages and the verified official V15 package by default;
- other packages require an explicit force override;
- the VoiceTest firmware identity, package layout, loader compatibility and recovery path do not exist yet.

Do not use `--force` as a normal installation strategy. Do not send a package to the FM-1 until its target model, package identity and recovery path have been checked.

## Planned installer/updater design

1. **Read-only detection:** identify the connected device and report its firmware identity before offering any write operation.
2. **Compatibility manifest:** every VoiceTest release declares supported FM-1 model/flash variants, source firmware range, package format and minimum installer version.
3. **Preflight:** verify package structure, size, SHA-256, model and compatibility before the device is asked to enter update mode.
4. **Backup:** provide a verified backup/export path for user data where the installed firmware supports it. Never claim a full firmware backup unless the method actually reads the complete flash image.
5. **Install:** use a documented USB update path and a package-specific loader. Keep the existing boot/recovery region untouched unless the upstream protocol explicitly requires otherwise.
6. **Post-write verification:** reconnect, read the reported identity and compare it with the package manifest.
7. **Recovery:** document a tested return path to original Felucca and, separately, official M-VAVE V15. A rollback path is not considered supported until tested on hardware.
8. **Update:** check signed or otherwise integrity-verified release metadata, download to the computer first, validate the complete package, then ask for confirmation before writing.

## Build strategy

- Keep the portable DSP and protocol code independent of the JieLi SDK.
- Inspect the current Felucca build, BSP submodule and exact AC79 SDK revision before adding FM-1 platform files.
- Keep third-party source and licence notices intact. Felucca is GPL-3.0-only; its hardware layer and engine submodules have their own licences.
- First target milestone: compile a harmless VoiceTest application against the correct FM-1 toolchain without flashing it.
- Second target milestone: run it on hardware with audio output/bypass only.
- Only after that, add and measure a phone-audio receiver.

## Current status

This document is an integration plan, not a claim that VoiceTest can yet be installed on the FM-1. No VoiceTest firmware package or device-side update loader has been produced or tested.
