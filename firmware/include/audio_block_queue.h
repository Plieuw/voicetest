# VoiceTest

Experimental voice-analysis and voice-processing firmware for the M-VAVE FM1.

**Status:** portable DSP and phone-packet parser scaffolding. There is not yet a working FM1 application image, device-side Wi-Fi receiver, or verified wireless microphone path.

## Goal

Build a real-time voice engine that analyses one incoming voice signal and shares that analysis with multiple processing blocks:

- pitch detection and pitch correction (AutoTune-style)
- pitch shifting
- frequency shifting (not the same as pitch shifting)
- vocoder processing
- formant analysis, shifting and morphing
- experimental SAM-like synthetic speech driven by live voice features

The aim is a flexible voice-processing instrument, not a single-purpose vocoder.

## Design principles

1. One shared analysis engine. Effects reuse pitch, spectral and voice-state information rather than each implementing a separate analyser.
2. Modular processing. Processing blocks are independently testable and eventually routable in different combinations.
3. Portable DSP first. Keep the core independent of the FM1 SDK.
4. Transport-independent audio. Wi-Fi, USB audio and any future transport feed the same PCM interface.
5. No assumed hardware capabilities. A transport is supported only after it is demonstrated on the target.
6. Original Felucca first. Original Felucca remains the user's starting firmware and recovery reference. VoiceTest is a separate clean app, not a modification of Felucca or fm1pokoe.

## Repository layout

- firmware/include/ — public DSP and transport interfaces
- firmware/src/ — portable DSP and packet parsing implementation
- tests/ — host-side tests
- phone-app/android/ — experimental Android microphone sender
- docs/ARCHITECTURE.md — system design
- docs/AUDIO_TRANSPORT.md — input transport investigation
- docs/DSP_ENGINE.md — analysis and processing design
- docs/ROADMAP.md — staged implementation plan
- docs/FM1_TARGET.md — FM1 hardware and resource notes
- docs/FELUCCA_INTEGRATION.md — Felucca baseline, platform and installer/updater plan
- docs/PHONE_AUDIO_PROTOCOL.md — VTP1 phone audio protocol

## Phone microphone input

The Android companion app in phone-app/android/ captures the phone microphone only after the user presses Start and sends mono PCM16 over local Wi-Fi UDP. The packet format is documented in docs/PHONE_AUDIO_PROTOCOL.md.

The portable VTP1 parser is implemented in firmware/src/phone_audio_packet.c. It validates the fixed 48 kHz / 20 ms datagram and exposes PCM16 samples as floats without allocation or copying. The FM1-side UDP socket receiver, jitter buffer and audio callback integration are not implemented yet. The Android project has not yet been built on a physical phone.

## Build the portable core

Requires CMake and a C compiler:

    cmake -S . -B build
    cmake --build build
    ctest --test-dir build --output-on-failure

The portable core currently calculates basic signal measurements and includes an initial YIN-based F0 estimator. Its caller-owned scratch workspace is intended to live in static storage on the FM1, not on the audio callback stack. The phone packet parser has host-side tests for packet validation and signed PCM conversion. Neither DSP nor networking has been profiled on the FM1.

## FM1 platform

The practical route for phone Wi-Fi is a clean VoiceTest app on fm1-base plus felucca-bsp. Felucca itself is the user's starting firmware and recovery reference; its GPL application code is not copied into the Wi-Fi app. See docs/FELUCCA_INTEGRATION.md before any installation work.

## Project separation

This is a separate project. It does not modify fm1pokoe or the upstream Felucca repository.
