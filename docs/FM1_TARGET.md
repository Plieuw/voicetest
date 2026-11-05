# FM1 target notes

## Starting point: original Felucca

The integration baseline is the original upstream Felucca firmware and source, not the user's `fm1pokoe` fork.

The current upstream Felucca project documents:
- a firmware app, a dedicated update loader and a `.fwsc` install package
- a USB-MIDI SysEx installer with device identity detection and post-install identity verification
- a route back to the official M-VAVE V15 package
- a build based on the JieLi pi32v2 toolchain and AC79 SDK
- separate `Felucca-Engines` and `felucca-bsp` submodules

See [FELUCCA_INTEGRATION.md](FELUCCA_INTEGRATION.md) and the upstream [BUILDING.md](https://github.com/hugelton/Felucca/blob/main/BUILDING.md).

Do not assume that Felucca's installer accepts an arbitrary VoiceTest package. Its current installer validates package identity and the Felucca loader marker; the official V15 package is a special, hash-verified exception. A VoiceTest package format and update path must be designed and tested separately.

## Known hardware constraints

The FM1 is based on the JieLi AC791N / WL82 family and uses a pi32v2 CPU. Community hardware documentation reports:
- two CPU cores and single-precision hardware floating point
- approximately 512 KB of usable on-chip SRAM in the firmware memory window
- no external RAM
- stereo audio output through the ALNK0/I2S path
- USB normally operating as a device; USB host mode may be possible with external power
- no dedicated debug header on the board

Exact chip markings and flash variants have differed between reported board revisions. Treat these as platform notes, not a substitute for checking the actual unit.

## Wi-Fi resource and Bluetooth constraint

The current `fm1-base` documentation describes Wi-Fi as a station using JieLi's Wi-Fi libraries and lwIP sockets. Its Wi-Fi memory map reserves about 128 KiB for Wi-Fi library buffers, and the example build explicitly disables USB audio input because RAM is needed by Wi-Fi. The same platform currently builds with Bluetooth or Wi-Fi, not both. Therefore, while Wi-Fi UDP is a plausible phone-audio path, it has a substantial memory cost and means BLE-MIDI cannot be used at the same time in that platform configuration.

## Audio input is the key unknown

The published board overview documents stereo output, but does not establish a built-in microphone or line-level audio input. The FM1 should therefore not be assumed to accept an analogue microphone directly.

Felucca's current build guide describes USB Audio Class input as the master output (stereo, 44.1 or 48 kHz) exposed over USB. That is useful for sending FM1 audio to a computer, but it does not by itself prove that the FM1 application can consume incoming PCM from a phone or computer. We must inspect the BSP's actual capture/playback direction and callback before treating USB audio as a usable input.

## Candidate input routes

1. **Wi-Fi UDP from a companion device.** Technically plausible if the firmware can receive PCM and the RAM/flash budget permits it. Memory cost and radio coexistence need measurement on the exact Felucca/BSP baseline.
2. **USB Audio Class host input.** Requires verified host-side audio capture and a suitable power/host arrangement; ordinary USB device audio descriptors do not establish this.
3. **External audio interface / custom hardware.** Not part of the no-modification plan; do not assume a direct analogue input exists.
4. **Bluetooth audio input.** Not currently established by the reviewed Felucca build documentation. Do not confuse Bluetooth MIDI with audio streaming.

## First hardware milestone

Before flashing a custom image:
- record the firmware version shown at power-on
- back up projects/presets and confirm what the backup actually contains
- confirm the exact target board/flash revision
- pin the matching Felucca source, BSP and SDK revisions
- build and test a harmless audio-output prototype first
- only then test a candidate voice-input route

Do not flash an experimental image until a recovery path and a known-good original Felucca package are available and verified.
