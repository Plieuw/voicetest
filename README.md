# VoiceTest

Experimental voice-analysis and voice-processing firmware for the M-VAVE FM1.

**Status:** early architecture and portable DSP-core scaffold. This repository does not yet contain a working FM1 firmware image, device-specific drivers, or a verified wireless microphone path.

## Goal

Build a real-time voice engine that can analyse one incoming voice signal and make that shared analysis available to multiple processing blocks, including:

- pitch detection and pitch correction (AutoTune-style)
- pitch shifting
- frequency shifting (not the same as pitch shifting)
- vocoder processing
- formant analysis, shifting and morphing
- experimental SAM-like synthetic speech driven by live voice features

The aim is a flexible voice-processing instrument, not a single-purpose vocoder.

## Design principles

1. **One shared analysis engine.** Effects should reuse the same pitch, spectral and voice-state information instead of each implementing a separate analyser.
2. **Modular processing.** Processing blocks should be independently testable and eventually routable in different combinations.
3. **Portable DSP first.** Keep the core independent of the FM1 SDK so it can be built and tested on a desktop before device integration.
4. **Transport-independent audio.** Wi-Fi, USB audio and any future transport feed the same PCM interface.
5. **No assumed hardware capabilities.** A transport or codec is not considered supported until it is demonstrated on the target hardware and firmware.

## Repository layout

- `firmware/include/` — public DSP interfaces
- `firmware/src/` — portable DSP implementation
- `tests/` — host-side tests
- `docs/ARCHITECTURE.md` — system design
- `docs/AUDIO_TRANSPORT.md` — input transport investigation
- `docs/DSP_ENGINE.md` — analysis and processing design
- `docs/ROADMAP.md` — staged implementation plan

## Build the portable core

Requires CMake and a C compiler:

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

The current core is deliberately small: it accepts mono floating-point PCM frames and calculates basic signal measurements. It is a foundation for the shared analyser, not yet a pitch/formant detector.

## Hardware integration

The FM1-specific layer will be added only after confirming the exact board revision, SoC/SDK interfaces, available RAM/CPU budget, and audio input/output paths. See the transport notes before assuming that USB audio input, Wi-Fi audio streaming, or Bluetooth LE Audio works on the device.

## Project separation

This is a new project. It is not a continuation or rewrite of `fm1pokoe`, and it does not copy assumptions from that project.