# VoiceTest

Experimental voice-analysis and voice-processing firmware for the M-VAVE FM1.

**Status:** portable DSP, VTP1 packet parsing and a bounded real-time audio queue are implemented with host tests. The FM1 firmware integration, device-side network receiver and verified wireless microphone path are not yet complete.

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

- `firmware/include/` — public DSP, packet and audio-queue interfaces
- `firmware/src/` — portable DSP, packet parsing and bounded audio queue
- `tests/` — host-side tests
- `docs/ARCHITECTURE.md` — system design
- `docs/AUDIO_TRANSPORT.md` — input transport investigation
- `docs/DSP_ENGINE.md` — analysis and processing design
- `docs/ROADMAP.md` — staged implementation plan
- `docs/FM1_TARGET.md` — hardware constraints and audio-input investigation
- `docs/FELUCCA_INTEGRATION.md` — original Felucca baseline and installer/updater plan
- `docs/AUDIO_QUEUE.md` — SPSC queue contract and real-time integration notes

## Phone microphone input

An experimental Android companion app is in `phone-app/android/`. It captures the phone microphone only after the user presses Start and sends mono PCM16 over local Wi-Fi UDP. The packet format is documented in `docs/PHONE_AUDIO_PROTOCOL.md`.

The portable VTP1 packet parser validates the fixed 48 kHz / 20 ms packet format. The `vt_audio_block_queue` copies validated PCM into a four-block single-producer/single-consumer queue for a network task and audio consumer. The queue is bounded, uses acquire/release publication, performs no allocation, and rejects packets when full. Its host test covers ordering, overflow, empty reads and signed PCM values.

**Not implemented yet:** FM1 Wi-Fi socket receiver, jitter/loss recovery policy, audio ISR hookup, DSP effects on device, and end-to-end phone-to-FM1 testing. The Android project has not yet been verified on a physical device.

## Build the portable core

Requires CMake and a C compiler:

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

The current core accepts mono floating-point PCM frames, calculates basic signal measurements, and includes an initial YIN-based F0 estimator with a confidence value. The phone packet parser and audio queue have host-side tests. These components have not yet been profiled on the FM1; formant and phoneme analysis are not implemented.

## Build the original Felucca baseline

The upstream source, BSP and engines revisions are pinned in [docs/UPSTREAM_PINS.md](docs/UPSTREAM_PINS.md). To reproduce the untouched Felucca build (toolchain and AC79 SDK must already be installed as described in upstream `BUILDING.md`):

```sh
sh tools/build_felucca_baseline.sh
```

This only builds the upstream baseline in `build/upstream/Felucca`; it does not flash the device and is not yet the VoiceTest firmware.

## Hardware integration

The FM1-specific layer will be added only after confirming the exact board revision, SoC/SDK interfaces, available RAM/CPU budget, and audio input/output paths. The integration baseline is original Felucca, not a guessed generic AC79 port. See [the Felucca integration plan](docs/FELUCCA_INTEGRATION.md) before any firmware installation work.

## Project separation

This is a new project. It is not a continuation or rewrite of `fm1pokoe`, and it does not modify the upstream Felucca repository.
