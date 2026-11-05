# Audio transport investigation

This document separates possible transports from confirmed capabilities. No wireless microphone input is considered working until a complete end-to-end test has been made on the FM1.

## Internal interface

All input adapters should deliver timestamped PCM blocks to the same queue. The first portable-core target is mono float PCM. A transport may use another wire format and convert at the boundary.

## Candidates

| Transport | Intended role | Current assessment |
|---|---|---|
| Wi-Fi UDP | Wireless microphone audio from an Android companion app | Primary experiment. Requires a working Wi-Fi network interface and firmware access; packet loss, jitter, power and latency must be measured. |
| USB Audio | Wired audio input | Investigate device role and descriptors. Evidence that a unit exposes USB Audio output does not prove it can act as a USB host for a microphone. |
| BLE-MIDI | Notes, CC, pitch bend and control | Suitable for control events, not continuous uncompressed voice audio. Keep it separate from the PCM stream. |
| Bluetooth Classic A2DP | Audio playback/output | Existing project observations suggest it may be available as an output path, but codec buffering can make live monitoring feel delayed. Measure on the actual build. |
| Bluetooth LE Audio | Possible future low-latency audio link | Experimental only. Bluetooth 5.x branding alone does not establish LC3, ISO/CIS, or BAP support in the chip SDK and firmware. |

## Wi-Fi UDP experiment

A companion Android app could capture microphone PCM and send sequenced packets to the FM1. A packet header should include at least:
- stream/session identifier
- monotonically increasing sequence number
- sample counter or timestamp
- payload sample count and format

The receiver should detect missing, late and reordered packets. A small adaptive or configurable jitter buffer can smooth arrival variation, but it adds latency. Measure packet loss and round-trip/one-way latency under realistic Wi-Fi conditions.

Do not assume a particular sample rate, packet duration, or latency target until the device's CPU, network stack and audio callback have been profiled.

## USB investigation checklist

Before implementing USB input, establish:
1. Is the FM1 USB port operating as a host, device, or switchable dual-role port?
2. Does the hardware expose host power and the required data lines?
3. Does the SDK contain USB Audio Class host support, not merely device support?
4. Can a standard USB microphone or class-compliant interface enumerate?
5. What sample rates, formats and channel counts are accepted?

If the device only exposes USB Audio as a device, a phone or computer may be able to send audio to it only if the firmware implements the appropriate USB device-side audio function. That is a separate design from plugging a USB microphone into the FM1.

## BLE-MIDI

Use BLE-MIDI for discrete musical and control data:
- note on/off
- CC for effect parameters
- pitch bend
- clock/transport where supported

Do not try to tunnel the microphone waveform through MIDI events. Its event encoding and throughput are not designed for continuous PCM.

## LE Audio verification

Before treating LE Audio as a real option, locate and test the actual SDK APIs and controller support for LC3 and Bluetooth ISO transport (CIS/BIS as applicable). A list of general audio codecs in an SDK is not proof of LE Audio support.

## Required measurements

For each viable path, record:
- end-to-end input latency and variation
- packet loss / audio dropouts
- CPU load and memory use
- sustained operation time
- behaviour when the link reconnects
- output monitoring latency

The first milestone is reliable audio capture, not the lowest theoretical latency.