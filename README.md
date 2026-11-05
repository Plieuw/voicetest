# VoiceTest

Experimental voice-analysis and voice-processing firmware for the M-VAVE FM1.

**Status:** early portable DSP and transport-protocol scaffolding. This repository does not yet contain a working FM1 firmware image, device-specific audio/network drivers, or a verified wireless microphone path.

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
6. **Original Felucca first.** Use the original Felucca source/build as the hardware integration baseline. Keep this project separate from `fm1pokoe` and upstream Felucca.

## Repository layout

- `firmware/include/` — public DSP and transport interfaces
- `firmware/src/` — portable DSP and packet parsing implementation
- `tests/` — host-side tests
- `docs/ARCHITECTURE.md` — system design
- `docs/AUDIO_TRANSPORT.md` — input transport investigation
- `docs/DSP_ENGINE.md` — analysis and processing design
- `docs/ROADMAP.md` — staged implementation plan
- `docs/FM1_TARGET.md` — hardware constraints and audio-input investigation
- `docs/FELUCCA_INTEGRATION.md` — original Felucca baseline and installer/updater plan

## Phone microphone input

An experimental Android companion app is in `phone-app/android/`. It captures the phone microphone only after the user presses Start and sends mono PCM16 over local Wi-Fi UDP. The packet format is documented in `docs/PHONE_AUDIO_PROTOCOL.md`.

The portable VTP1 packet parser is now implemented in `firmware/src/phone_audio_packet.c`. It validates the fixed 48 kHz / 20 ms packet format and exposes PCM16 samples as floats without allocation or copying. The FM1-side socket receiver, jitter buffer and audio callback integration are not implemented yet. The Android project has not yet been built on a physical device.

## Build the portable core

Requires CMake and a C compiler:

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

The current core accepts mono floating-point PCM frames, calculates basic signal measurements, and includes an initial YIN-based F0 estimator with a confidence value. The phone packet parser has host-side tests for packet validation and signed PCM conversion. These components have not yet been profiled on the FM1; formant and phoneme analysis are not implemented.

## Hardware integration

The FM1-specific layer will be added only after confirming the exact board revision, SoC/SDK interfaces, available RAM/CPU budget, and audio input/output paths. The integration baseline is original Felucca, not a guessed generic AC79 port. See [the Felucca integration plan](docs/FELUCCA_INTEGRATION.md) before any firmware installation work.

## Project separation

This is a new project. It is not a continuation or rewrite of `fm1pokoe`, and it does not modify the upstream Felucca repository.
