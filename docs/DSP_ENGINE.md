# Shared DSP engine

## Purpose

The analyser produces a common set of time-aligned voice features for all effects. Pitch correction, vocoding and formant processing should not each maintain unrelated pitch or spectral analysis pipelines unless a later algorithm has a specific reason.

## Feature stages

### Stage 0 — signal measurements

Implemented in the initial portable scaffold:
- peak absolute amplitude
- RMS level
- zero-crossing rate

These measurements are useful for level tracking and basic diagnostics. They are not sufficient for pitch or phoneme recognition.

### Stage 1 — fundamental frequency and voicing

Initial implementation: a YIN-style difference-function estimator is now present in the portable core. It returns an F0 estimate, a confidence value, and a voiced flag. It uses single-precision arithmetic and fixed stack storage. It is an initial prototype, not yet tuned or benchmarked on the FM1.

Still planned:
- fundamental-frequency estimate (F0)
- confidence / periodicity
- voiced, unvoiced or uncertain classification
- attack and pitch-change tracking

The estimator must be robust to harmonics, vibrato, breath noise and changing vocal registers. A zero-crossing counter must not be used as a substitute for a proper F0 estimator.

### Stage 2 — spectral envelope and formants

Planned:
- windowed spectrum
- spectral envelope estimation
- formant frequency and bandwidth tracks, with confidence
- spectral tilt and harmonic/noise balance

Formants are resonances of the vocal tract; they are not interchangeable with the fundamental pitch. Pitch shifting and formant shifting therefore need separate controls and tests.

### Stage 3 — speech and transient features

Planned:
- voiced/unvoiced transitions
- consonant/transient detection
- amplitude envelope
- coarse phonetic feature estimates

A SAM-like synthesis mode could use these features to control a speech synthesiser. It should be treated as a separate synthesis subsystem, not as a normal effect that simply shifts the input waveform.

## Processing modules

| Module | Uses shared features | Notes |
|---|---|---|
| Pitch correction | F0, voicing, confidence | Quantises or smoothly attracts F0 to a selected scale; preserve natural transitions as a parameter. |
| Pitch shift | F0 and/or spectral representation | Changes perceived note while aiming to preserve duration. |
| Frequency shift | Usually analytic/quadrature signal path | Moves spectral components by a fixed frequency offset; unlike pitch shift, it is not a musical transposition. |
| Vocoder | Spectral envelope / band levels, plus carrier | Voice analysis controls a separate carrier source. |
| Formant shift/morph | Spectral envelope and formant tracks | Changes vocal-tract character independently of note where algorithmically possible. |
| SAM-like synthesis | Voicing, envelope and speech features | Experimental speech synthesis driven by live analysis. |

## API direction

The public API should keep audio frames and analysis features separate. An effect receives the feature frame as read-only input and must not silently modify the analyser's state.

The current header defines a small feature structure and a stateless frame-analysis function. Future analysis algorithms can extend the structure while keeping confidence and validity explicit.

## Quality and performance tests

For each new algorithm, add offline test signals:
- sine waves at known frequencies
- harmonic complexes
- vibrato and pitch glides
- noisy/unvoiced signals
- speech recordings with suitable permission
- silence and abrupt level changes

Measure accuracy, latency, CPU time, memory and behaviour at frame boundaries. Listening tests are essential; numeric accuracy alone does not guarantee a useful musical result.