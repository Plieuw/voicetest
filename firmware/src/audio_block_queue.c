#include "audio_block_queue.h"

#include <stddef.h>

#if !defined(__GNUC__) && !defined(__clang__)
#error "VoiceTest SPSC queue requires GCC/Clang __atomic builtins"
#endif

void vt_audio_queue_init(vt_audio_block_queue_t *queue)
{
    if (queue == NULL) return;
    queue->write_index = 0U;
    queue->read_index = 0U;
}

int vt_audio_queue_push(vt_audio_block_queue_t *queue,
                        const uint8_t *datagram,
                        uint32_t datagram_bytes)
{
    vt_phone_packet_t packet;
    uint32_t write_index, read_index;
    vt_audio_block_t *slot;
    size_t i;

    if (queue == NULL ||
        !vt_phone_packet_parse(datagram, (size_t)datagram_bytes, &packet)) {
        return 0;
    }

    write_index = __atomic_load_n(&queue->write_index, __ATOMIC_RELAXED);
    read_index = __atomic_load_n(&queue->read_index, __ATOMIC_ACQUIRE);
    if ((uint32_t)(write_index - read_index) >= VT_AUDIO_QUEUE_CAPACITY) {
        return 0;
    }

    slot = &queue->blocks[write_index % VT_AUDIO_QUEUE_CAPACITY];
    slot->sequence = packet.sequence;
    slot->first_sample = packet.first_sample;
    for (i = 0U; i < VT_PHONE_SAMPLES_PER_PACKET; ++i) {
        uint16_t raw = (uint16_t)packet.pcm_le[2U * i]
                     | (uint16_t)((uint16_t)packet.pcm_le[2U * i + 1U] << 8);
        slot->pcm[i] = raw >= 0x8000U
                     ? (int16_t)((int32_t)raw - 65536)
                     : (int16_t)raw;
    }

    /* Publish the complete slot only after all sample writes are visible. */
    __atomic_store_n(&queue->write_index, write_index + 1U, __ATOMIC_RELEASE);
    return 1;
}

int vt_audio_queue_pop(vt_audio_block_queue_t *queue, vt_audio_block_t *block)
{
    uint32_t read_index, write_index;
    const vt_audio_block_t *slot;
    size_t i;

    if (queue == NULL || block == NULL) return 0;

    read_index = __atomic_load_n(&queue->read_index, __ATOMIC_RELAXED);
    write_index = __atomic_load_n(&queue->write_index, __ATOMIC_ACQUIRE);
    if (read_index == write_index) return 0;

    slot = &queue->blocks[read_index % VT_AUDIO_QUEUE_CAPACITY];
    block->sequence = slot->sequence;
    block->first_sample = slot->first_sample;
    for (i = 0U; i < VT_PHONE_SAMPLES_PER_PACKET; ++i) {
        block->pcm[i] = slot->pcm[i];
    }

    /* The producer can reuse the slot only after the copy is complete. */
    __atomic_store_n(&queue->read_index, read_index + 1U, __ATOMIC_RELEASE);
    return 1;
}

uint32_t vt_audio_queue_count(const vt_audio_block_queue_t *queue)
{
    uint32_t write_index, read_index;
    if (queue == NULL) return 0U;
    write_index = __atomic_load_n(&queue->write_index, __ATOMIC_ACQUIRE);
    read_index = __atomic_load_n(&queue->read_index, __ATOMIC_ACQUIRE);
    return (uint32_t)(write_index - read_index);
}
