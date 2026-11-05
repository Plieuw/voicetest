#include "audio_block_queue.h"

#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "FAIL: %s (line %d)\n", #condition, __LINE__); \
        return 1; \
    } \
} while (0)

static void put_u16(uint8_t *p, uint16_t v)
{
    p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8);
}
static void put_u32(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8);
    p[2] = (uint8_t)(v >> 16); p[3] = (uint8_t)(v >> 24);
}
static void put_u64(uint8_t *p, uint64_t v)
{
    put_u32(p, (uint32_t)v); put_u32(p + 4, (uint32_t)(v >> 32));
}
static void make_packet(uint8_t *p, uint32_t sequence, int16_t first)
{
    size_t i;
    memset(p, 0, VT_PHONE_DATAGRAM_BYTES);
    memcpy(p, "VTP1", 4U);
    p[4] = VT_PHONE_PACKET_VERSION;
    put_u16(p + 6, VT_PHONE_HEADER_BYTES);
    put_u32(p + 8, sequence);
    put_u64(p + 12, (uint64_t)sequence * VT_PHONE_SAMPLES_PER_PACKET);
    for (i = 0U; i < VT_PHONE_SAMPLES_PER_PACKET; ++i) {
        put_u16(p + VT_PHONE_HEADER_BYTES + 2U * i,
                (uint16_t)(int16_t)(first + (int16_t)i));
    }
}

int main(void)
{
    vt_audio_block_queue_t queue;
    vt_audio_block_t block;
    uint8_t packet[VT_PHONE_DATAGRAM_BYTES];
    uint32_t i;

    vt_audio_queue_init(&queue);
    CHECK(vt_audio_queue_count(&queue) == 0U);
    CHECK(vt_audio_queue_pop(&queue, &block) == 0);

    for (i = 0U; i < VT_AUDIO_QUEUE_CAPACITY; ++i) {
        make_packet(packet, i, (int16_t)(-1000 + (int32_t)i));
        CHECK(vt_audio_queue_push(&queue, packet, sizeof(packet)) == 1);
    }
    CHECK(vt_audio_queue_count(&queue) == VT_AUDIO_QUEUE_CAPACITY);
    make_packet(packet, 99U, 0);
    CHECK(vt_audio_queue_push(&queue, packet, sizeof(packet)) == 0);

    for (i = 0U; i < VT_AUDIO_QUEUE_CAPACITY; ++i) {
        CHECK(vt_audio_queue_pop(&queue, &block) == 1);
        CHECK(block.sequence == i);
        CHECK(block.first_sample == (uint64_t)i * VT_PHONE_SAMPLES_PER_PACKET);
        CHECK(block.pcm[0] == (int16_t)(-1000 + (int32_t)i));
        CHECK(block.pcm[VT_PHONE_SAMPLES_PER_PACKET - 1U] ==
              (int16_t)(-1000 + (int32_t)i + (int32_t)VT_PHONE_SAMPLES_PER_PACKET - 1));
    }
    CHECK(vt_audio_queue_count(&queue) == 0U);
    CHECK(vt_audio_queue_pop(&queue, &block) == 0);

    make_packet(packet, 7U, INT16_MIN);
    CHECK(vt_audio_queue_push(&queue, packet, sizeof(packet) - 1U) == 0);
    CHECK(vt_audio_queue_push(&queue, packet, sizeof(packet)) == 1);
    CHECK(vt_audio_queue_pop(&queue, &block) == 1);
    CHECK(block.pcm[0] == INT16_MIN);

    puts("All audio block queue tests passed.");
    return 0;
}
