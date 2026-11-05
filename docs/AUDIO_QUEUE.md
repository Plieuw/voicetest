# Real-time audio block queue

`vt_audio_block_queue` is the boundary between a packet-receive task and an audio consumer.

## Contract

- Single producer: the network/transport task.
- Single consumer: the audio render callback or ISR.
- Four fixed slots, each holding one VTP1 block (960 mono PCM16 samples, 20 ms).
- Storage: 4 × 960 × 2 = 7,680 bytes, plus small metadata.
- No heap allocation, locks, logging, socket calls or waiting in the queue functions.
- Producer publishes a slot with a release store; consumer observes it with an acquire load. The reverse index hand-off prevents the producer from reusing a slot before the consumer has copied it.
- A full queue rejects the new packet. An empty queue returns no block; the audio side must use its own defined underrun policy (initially fade to silence), not wait.

The indices are monotonically increasing unsigned 32-bit counters. Their difference is used for occupancy, so wraparound is safe while the queue never holds more than four entries.

## Important limits

This is a portable queue primitive, not the FM1 network receiver. It assumes exactly one producer and one consumer; multiple socket tasks or multiple audio consumers need external serialization or a different queue. It also assumes the compiler provides GCC/Clang `__atomic` builtins with 32-bit acquire/release operations. The target build must verify those operations are lock-free on the JieLi toolchain.

The queue does not reorder packets, conceal loss, or adapt latency. Those belong in the network/jitter-buffer layer. It preserves the sequence number and first-sample counter so that layer can detect gaps and discontinuities before enqueueing.

## Felucca integration

Felucca's current audio renderer runs from the ALNK0 interrupt. Network receive must remain outside that interrupt. The intended wiring is:

```text
Wi-Fi socket task
  -> validate VTP1 datagram
  -> enqueue block
  -> audio render callback pops one block
  -> PCM16 to DSP input
  -> processing chain
```

The original Felucca source currently does not provide the Wi-Fi socket task shown above. A device-side network transport still needs to be ported and measured; this queue alone does not enable phone audio.
