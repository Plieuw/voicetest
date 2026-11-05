# FM1 target notes

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

The fm1-base USB audio options also need to be interpreted carefully:
- USB audio recording exposes the FM1's output to a computer.
- USB audio playback lets a computer play through the FM1's output mixer.
- The platform documentation says the computer's playback is mixed after the app's own MASTER output. That does not establish that an app can read the computer's incoming PCM as an input signal.

A real-time voice processor needs a PCM input that the app can actually access. The current fm1-base app interface documents USB audio capture as a platform feature, but its Wi-Fi example disables it for memory reasons. We must inspect the exact audio callback/API and verify whether app code can consume those captured samples before treating USB audio as a usable input.

## Candidate input routes

1. **Wi-Fi UDP from a companion device.** Technically plausible if the firmware can receive PCM and the RAM/flash budget permits it. On fm1-base, Wi-Fi and Bluetooth are mutually exclusive. The Wi-Fi stack consumes a substantial part of the available memory, so this needs a measured prototype.
2. **USB Audio Class host input.** Potentially possible only if the FM1 is powered and configured as a USB host and the firmware implements host-side audio capture. This is not provided by the ordinary USB audio device features described above.
3. **External audio interface / custom hardware.** Not part of the no-modification plan; do not assume a direct analogue input exists.
4. **Bluetooth audio input.** Not currently established by the reviewed fm1-base documentation. Its documented A2DP feature is output to headphones/speakers.

## First hardware milestone

Before flashing a custom image:
- record the firmware version shown at power-on
- make a full backup of the current firmware and user data with a compatible tool
- confirm the exact target platform and matching SDK/toolchain
- build and test a harmless audio-output prototype first
- only then test a candidate voice-input route

Do not flash an experimental image until a recovery path and a known-good stock image are available.
