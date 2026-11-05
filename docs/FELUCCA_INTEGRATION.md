# Original Felucca and FM1 platform integration

## User's starting point

The user's FM1 is first returned to original Felucca. Keep that as the starting firmware and recovery reference. VoiceTest remains a separate repository; it must not modify fm1pokoe or upstream Felucca.

Upstream references:
- Felucca: https://github.com/hugelton/Felucca
- Felucca build guide: https://github.com/hugelton/Felucca/blob/main/BUILDING.md
- Felucca installer: https://hugelton.github.io/Felucca/webapp/installer/
- Shared hardware layer: https://github.com/hugelton/felucca-bsp
- FM1 platform with Wi-Fi: https://github.com/charlesvestal/fm1-base

## Existing Felucca update system

Felucca has a firmware app, update loader and .fwsc package. Its USB-MIDI SysEx installer detects the FM1 identity, transfers the package in two stages, waits for the loader/reboot and verifies the resulting identity. It also has a return path to the official, unmodified M-VAVE V15 package using a SHA-256 check.

This is a useful reference for the VoiceTest installer/updater, but it is not a drop-in installer for arbitrary VoiceTest packages. The upstream installer validates package identity and the Felucca loader marker; the official V15 package is a special hash-verified exception. A VoiceTest package format and update path still need to be designed and tested.

Do not use --force as a normal installation strategy. Do not write a package until its target model, package identity and recovery path have been checked.

## Hardware platform decision

The current Felucca hardware layer, felucca-bsp, supplies the FM1 chip and board drivers, but its README places Wi-Fi support in the separate fm1-base platform. fm1-base is built on felucca-bsp and exposes:
- app_render() for the audio output callback
- plat_wifi_on() and Wi-Fi state/join functions
- plat_net_task() for blocking network work outside the audio callback
- lwIP sockets for UDP/TCP in app network source files

The fm1-base Wi-Fi example uses wifi = true and net = ["net.c"] in fm1.toml. A network task can own a UDP socket and pass validated audio into a fixed-capacity ring; the audio callback must only consume ready samples. No socket call, waiting, allocation or logging belongs in the audio callback.

## Why VoiceTest should be a clean app

Felucca is GPL-3.0-only. The fm1-base porting guide documents a clean-port/provenance process and says that using JieLi's binary Wi-Fi libraries requires an MPL-2.0 firmware; Felucca's GPL application code cannot simply be copied into such a new app. The safe engineering direction is:
- keep original Felucca installed as the user's baseline and recovery reference;
- build VoiceTest as a new, independently written app on fm1-base and felucca-bsp;
- use the shared hardware/platform layers under their own licences;
- keep VoiceTest's DSP and app code original, with provenance and licence choices documented before distributing a binary.

This is a platform choice, not a claim that Felucca's synthesizer engine or UI has been ported.

## Wi-Fi trade-offs

The fm1-base documentation states that Wi-Fi and Bluetooth are mutually exclusive in the current platform configuration. Its Wi-Fi example disables USB audio input/output to make room for the radio. The documented Wi-Fi example uses roughly 420 KB of the 581 KB app slot, 119 KB of 128 KB data RAM and 299 KB of 304 KB buffer pool. That leaves a tight budget for voice DSP. The phone stream itself is modest (48 kHz mono PCM16 is 96 kB/s), but the radio stack's memory footprint is the limiting factor.

For VoiceTest this means:
- Wi-Fi first; do not plan on BLE-MIDI in the same build unless the platform changes and is measured.
- Disable USB audio features in the Wi-Fi build.
- Keep the phone receiver small and allocation-free after startup.
- Profile the DSP and radio together on the actual FM1 before adding expensive effects.
- Consider lower phone sample rates only after measuring voice quality and pitch-tracking accuracy.

## Planned installer/updater design

1. Read-only detection: identify the connected device and report its firmware identity before offering any write operation.
2. Compatibility manifest: every VoiceTest release declares supported FM1 model/flash variants, source firmware range, package format and minimum installer version.
3. Preflight: verify package structure, size, SHA-256, model and compatibility before the device is asked to enter update mode.
4. Backup: provide a verified backup/export path for user data where the installed firmware supports it. Never claim a full firmware backup unless the method actually reads the complete flash image.
5. Install: use a documented USB update path and a package-specific loader. Keep the existing boot/recovery region untouched unless the upstream protocol explicitly requires otherwise.
6. Post-write verification: reconnect, read the reported identity and compare it with the package manifest.
7. Recovery: document a tested return path to original Felucca and, separately, official M-VAVE V15. A rollback path is not supported until tested on hardware.
8. Update: check integrity-verified release metadata, download to the computer first, validate the complete package, then ask for confirmation before writing.

## Next build steps

1. Pin the exact fm1-base, felucca-bsp and AC79 SDK revisions.
2. Create a clean VoiceTest app from the fm1-base template, with Wi-Fi enabled and USB audio disabled.
3. Add the phone UDP receiver as a network-task source file.
4. Connect its bounded PCM ring to a simple, low-CPU bypass/monitor path.
5. Build the firmware package on the host, inspect its identity and size, and run simulator/host tests.
6. Only after those checks, install on hardware with a known recovery path.

## Current status

The portable VTP1 packet parser and its host test exist. The Android sender is still an unbuilt prototype. No FM1 Wi-Fi receiver, FM1 application, VoiceTest firmware package or VoiceTest update loader has been produced or tested.
