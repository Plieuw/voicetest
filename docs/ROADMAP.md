# Roadmap

## Phase 0 — foundation

- [x] Create a separate repository and define project boundaries.
- [x] Establish a portable PCM frame contract.
- [x] Add basic signal measurements and host-side tests.
- [x] Add a portable VTP1 phone-packet parser and PCM16 conversion tests.
- [ ] Build and run the complete host test suite in CI.
- [ ] Confirm exact FM1 board revision and SoC marking.

## Phase 1 — original Felucca and platform

- [x] Inspect original Felucca README, build guide and USB-MIDI installer.
- [x] Inspect Felucca audio output and USB audio direction.
- [x] Inspect felucca-bsp and fm1-base Wi-Fi/network interfaces.
- [x] Document Wi-Fi, memory, radio and source/licence constraints.
- [ ] Pin exact fm1-base, felucca-bsp and AC79 SDK revisions.
- [ ] Confirm VoiceTest's source licence/provenance for the Wi-Fi build.
- [ ] Create a clean VoiceTest app from the fm1-base template.
- [ ] Build a harmless FM1 app with the matching toolchain; do not flash yet.

## Phase 2 — reliable audio input/output

- [ ] Add the phone UDP receiver as a network-task source.
- [ ] Add a bounded PCM ring between network task and audio callback.
- [ ] Implement dropout reporting, jitter handling and safe fade on timeout.
- [ ] Implement output bypass/monitoring.
- [ ] Measure end-to-end latency and sustained stability on hardware.

## Phase 3 — shared analysis

- [ ] Add windowing and spectral analysis.
- [ ] Add F0 estimation with confidence.
- [ ] Add voiced/unvoiced classification.
- [ ] Add spectral envelope and formant tracking.
- [ ] Add transient and speech-feature estimates.
- [ ] Profile analysis CPU and RAM with Wi-Fi enabled.

## Phase 4 — processing blocks

- [ ] Frequency shifter.
- [ ] Pitch shifter.
- [ ] Pitch correction.
- [ ] Vocoder.
- [ ] Formant processing.
- [ ] Experimental analysis-driven speech synthesis.

## Phase 5 — installer and instrument integration

- [ ] Define VoiceTest package identity and compatibility manifest.
- [ ] Implement read-only device/firmware detection.
- [ ] Implement package integrity and hardware preflight checks.
- [ ] Implement backup/export where the active firmware supports it.
- [ ] Implement update and post-write identity verification.
- [ ] Test return to original Felucca and separately to official M-VAVE V15.
- [ ] Add update discovery and a user-confirmed update flow.
- [ ] Add MIDI parameter control only if the selected radio transport allows it.
- [ ] Add parameter smoothing and preset/state handling.
- [ ] Add a modular routing layer after individual blocks are stable.
- [ ] Profile and optimise on the FM1.

## Acceptance criteria

A feature is not complete merely because it compiles. It must have:
- a repeatable test
- measured CPU and memory cost on target where relevant
- documented latency
- stable behaviour on edge cases
- a clear bypass/failure mode
