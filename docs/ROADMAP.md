# Roadmap

## Phase 0 — foundation

- [x] Create a separate repository and define project boundaries.
- [x] Establish a portable PCM frame contract.
- [x] Add basic signal measurements and host-side tests.
- [ ] Build and run the host test suite in CI.
- [ ] Confirm exact FM1 board revision and SoC marking.

## Phase 1 — hardware facts

- [ ] Inspect the current FM1 firmware and SDK entry points.
- [ ] Confirm audio input and output hardware paths.
- [ ] Confirm available RAM, CPU budget and supported sample rates.
- [ ] Determine USB host/device capabilities.
- [ ] Prove Wi-Fi availability and a stable UDP receive path.
- [ ] Measure baseline audio callback timing.

## Phase 2 — reliable audio input/output

- [ ] Implement one verified input adapter.
- [ ] Implement bounded buffering and dropout reporting.
- [ ] Implement output adapter and bypass monitoring.
- [ ] Measure end-to-end latency and sustained stability.

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

## Phase 5 — instrument integration

- [ ] Add BLE-MIDI parameter control if verified.
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