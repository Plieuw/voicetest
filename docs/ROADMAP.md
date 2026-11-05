# Roadmap

## Phase 0 — foundation

- [x] Create a separate repository and define project boundaries.
- [x] Establish a portable PCM frame contract.
- [x] Add basic signal measurements and host-side tests.
- [x] Add a portable VTP1 phone-packet parser and PCM16 conversion tests.
- [ ] Build and run the host test suite in CI.
- [ ] Confirm exact FM1 board revision and SoC marking.

## Phase 1 — original Felucca integration

- [x] Inspect original Felucca README, build guide and USB-MIDI installer.
- [x] Document the existing package, loader, identity-check and official-V15 return path.
- [ ] Inspect the exact Felucca BSP submodule APIs used by the current release.
- [ ] Pin a compatible Felucca source revision, BSP revision and AC79 SDK revision.
- [ ] Build a harmless VoiceTest app with the matching FM1 toolchain; do not flash yet.
- [ ] Confirm audio input and output hardware paths.
- [ ] Confirm available RAM, CPU budget and supported sample rates.

## Phase 2 — reliable audio input/output

- [ ] Implement one verified input adapter.
- [ ] Implement bounded buffering and dropout reporting.
- [ ] Implement output adapter and bypass monitoring.
- [ ] Measure end-to-end latency and sustained stability.
- [ ] Prove Wi-Fi availability and a stable UDP receive path if Wi-Fi remains the selected transport.

## Phase 3 — shared analysis

- [ ] Add windowing and spectral analysis.
- [ ] Add F0 estimation with confidence.
- [ ] Add voiced/unvoiced classification.
- [ ] Add spectral envelope and formant tracking.
- [ ] Add transient and speech-feature estimates.

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
- [ ] Add BLE-MIDI parameter control only if the selected transport allows it.
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
