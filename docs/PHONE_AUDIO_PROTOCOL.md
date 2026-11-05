# Phone-to-FM1 audio stream

## First transport experiment

The phone companion app captures microphone audio only while the user presses Start. It sends mono PCM over UDP on the local Wi-Fi network. The FM1 receiver will be a separate firmware adapter.

This is experimental. It does not mean the FM1 UDP receiver is already implemented. In the current fm1-base platform, Wi-Fi and Bluetooth are mutually exclusive; Wi-Fi memory cost and packet throughput must be measured.

## Network

The phone and FM1 must be on the same local network, or on a network arrangement that allows UDP between them. Guest Wi-Fi client isolation may block this. The user enters the FM1's reachable IPv4 address.

Initial UDP port: 39000.

## PCM format

- 48,000 Hz, mono, signed 16-bit little-endian PCM
- 20 ms per packet, 960 samples
- 1,920-byte audio payload

This is a test target, not a proven FM1 limit. If necessary, test 24 kHz or 16 kHz and measure pitch-tracking and voice-quality impact.

## Datagram header

All fields are little-endian. Header size: 20 bytes.

| Offset | Size | Field |
|---:|---:|---|
| 0 | 4 | ASCII magic VTP1 |
| 4 | 1 | version, currently 1 |
| 5 | 1 | flags, currently 0 |
| 6 | 2 | header size, 20 |
| 8 | 4 | packet sequence number |
| 12 | 8 | first-sample counter since stream start |

PCM payload follows the header. Sequence gaps indicate packet loss; the sample counter supplies a timeline independent of packet arrival time.

## Receiver behaviour

- Validate magic, version, header size and payload length.
- Detect duplicate, late, missing and reordered packets.
- Use a bounded jitter buffer; do not allocate per packet.
- Convert PCM16 to portable float at the input boundary.
- Track loss, queue depth, underruns and latency.
- On stream timeout, fade to silence instead of replaying stale buffers.

## Privacy and security

This protocol is intended for a trusted local network. It does not encrypt or authenticate packets. Do not expose the receiver port to the public internet. The app shows streaming state and stops capture when Stop is pressed or the activity closes.

## Device-side next step

Implement the FM1 UDP receiver only after verifying SDK Wi-Fi APIs, task model, memory footprint and socket support. Network receive work must stay outside the audio interrupt and pass validated PCM through a fixed-capacity queue.
