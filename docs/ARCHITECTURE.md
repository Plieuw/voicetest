# System architecture

## Overview

The firmware is organised as a pipeline with a strict boundary between audio transport and DSP. Every transport produces the same internal PCM block format. The shared analyser runs once per input frame and publishes measurements to processing blocks.

```text
Audio source
   |
   v
Transport adapter
   |  PCM frames + timestamps
   v
Input queue / jitter handling
   |
   v
Shared voice analysis
   |-------------------------------|
   | pitch / voicing                |
   | spectral envelope / formants   |
   | level / transients             |
   | speech-state features          |
   |-------------------------------|
   |
   v
Processing graph
   | pitch correction
   | pitch shift
   | frequency shift
   | vocoder
   | formant processing
   | speech synthesis
   v
Output stage
```

The diagram describes the intended architecture. Most blocks are not implemented yet.

## Main subsystems

### 1. Transport adapters

A transport adapter handles device-specific I/O and converts incoming audio to the internal PCM representation. DSP code must not know whether audio arrived over Wi-Fi, USB, or a future audio link.

Each adapter should report:
- sample rate and channel count
- sample format
- packet or frame timestamp, where available
- discontinuities, overruns and dropped packets
- transport-specific latency estimates, if measured

### 2. Input queue

A bounded queue decouples packet arrival from audio processing. A network adapter may need a jitter buffer; a local synchronous audio interface may need only a small block queue. Buffer sizes must be measured against latency and dropout behaviour rather than guessed.

### 3. Shared voice analysis

Analysis runs once on the input signal (or on a clearly defined analysis tap). Its output is a time-aligned feature frame. Initial measurements can include peak, RMS and zero-crossing rate. Later stages can add robust fundamental-frequency estimation, voiced/unvoiced classification, spectral envelope and formant tracks.

Analysis results should include validity/confidence values. Effects must be able to handle missing or uncertain estimates without producing sudden jumps.

### 4. Processing graph

Effects consume audio and, where relevant, shared analysis features. A processing block should have:
- explicit input/output format
- bounded memory use
- a documented processing latency
- reset and parameter-update behaviour
- a bypass path

The first implementation can use a fixed chain. A graph/routing layer should be added only after the individual blocks and timing are understood.

### 5. Output stage

The output stage adapts the processed PCM to the FM1's verified output path. Monitoring latency is a system property: input transport, buffering, DSP, and output transport all contribute.

## Audio block contract

The portable core currently uses mono, 32-bit floating-point samples in the range conventionally expected to be around -1.0 to +1.0. The frame length and sample rate are supplied by the caller; neither is hard-coded as a hardware guarantee.

A future device adapter may convert signed 16-bit PCM or another native format into this representation. Candidate 48 kHz operation is a test target, not a confirmed FM1 configuration.

## Real-time rules

- No heap allocation in the audio processing callback.
- No file, network, logging or blocking operations in the audio processing callback.
- Use fixed-capacity buffers.
- Keep parameter changes thread-safe and click-free.
- Measure worst-case execution time on the actual target.
- Report underruns/overruns instead of silently hiding them.

## Portability boundary

The portable DSP library must compile without the JieLi SDK. FM1-specific code belongs in a separate platform layer, to be added once the exact SDK and board support are confirmed.