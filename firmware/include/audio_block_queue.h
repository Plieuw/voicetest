#ifndef VOICETEST_AUDIO_BLOCK_QUEUE_H
#define VOICETEST_AUDIO_BLOCK_QUEUE_H

#include "phone_audio_packet.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Four 20 ms blocks: 80 ms maximum queued audio, 7.5 KiB of PCM storage. */
#define VT_AUDIO_QUEUE_CAPACITY 4U

typedef struct {
    uint32_t sequence;
    uint64_t first_sample;
    int16_t pcm[VT_PHONE_SAMPLES_PER_PACKET];
} vt_audio_block_t;

typedef struct {
    /* SPSC indices: one network-task producer and one audio-ISR consumer. */
    volatile uint32_t write_index;
    volatile uint32_t read_index;
    vt_audio_block_t blocks[VT_AUDIO_QUEUE_CAPACITY];
} vt_audio_block_queue_t;

void vt_audio_queue_init(vt_audio_block_queue_t *queue);

/* Returns 1 when queued, 0 for an invalid packet or a full queue. */
int vt_audio_queue_push(vt_audio_block_queue_t *queue,
                        const uint8_t *datagram,
                        uint32_t datagram_bytes);

/* Copies the oldest block to caller-owned storage. Returns 0 when empty. */
int vt_audio_queue_pop(vt_audio_block_queue_t *queue,
                       vt_audio_block_t *block);

/* Number of complete blocks currently queued (approximate under concurrency). */
uint32_t vt_audio_queue_count(const vt_audio_block_queue_t *queue);

#ifdef __cplusplus
}
#endif

#endif
