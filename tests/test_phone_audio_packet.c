#include "phone_audio_packet.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "FAIL: %s (line %d)\n", #condition, __LINE__); \
        return 1; \
    } \
} while (0)

static void write_u16_le(uint8_t *p, uint16_t value)
{
    p[0] = (uint8_t)(value & 0xffU);
    p[1] = (uint8_t)(value >> 8);
}

static void write_u32_le(uint8_t *p, uint32_t value)
{
    p[0] = (uint8_t)value;
    p[1] = (uint8_t)(value >> 8);
    p[2] = (uint8_t)(value >> 16);
    p[3] = (uint8_t)(value >> 24);
}

static void write_u64_le(uint8_t *p, uint64_t value)
{
    write_u32_le(p, (uint32_t)value);
    write_u32_le(p + 4, (uint32_t)(value >> 32));
}

int main(void)
{
    uint8_t datagram[VT_PHONE_DATAGRAM_BYTES] = { 0U };
    vt_phone_packet_t packet;
    float sample = 0.0f;

    memcpy(datagram, "VTP1", 4U);
    datagram[4] = VT_PHONE_PACKET_VERSION;
    datagram[5] = 0U;
    write_u16_le(datagram + 6, VT_PHONE_HEADER_BYTES);
    write_u32_le(datagram + 8, 0x12345678U);
    write_u64_le(datagram + 12, UINT64_C(0x0102030405060708));

    /* First PCM sample = -32768; second = +32767. */
    write_u16_le(datagram + VT_PHONE_HEADER_BYTES, 0x8000U);
    write_u16_le(datagram + VT_PHONE_HEADER_BYTES + 2U, 0x7fffU);

    CHECK(vt_phone_packet_parse(datagram, sizeof(datagram), &packet) == 1);
    CHECK(packet.sequence == 0x12345678U);
    CHECK(packet.first_sample == UINT64_C(0x0102030405060708));
    CHECK(packet.sample_count == VT_PHONE_SAMPLES_PER_PACKET);
    CHECK(packet.pcm_le == datagram + VT_PHONE_HEADER_BYTES);

    CHECK(vt_phone_packet_sample(&packet, 0U, &sample) == 1);
    CHECK(fabsf(sample - (-1.0f)) < 0.00001f);
    CHECK(vt_phone_packet_sample(&packet, 1U, &sample) == 1);
    CHECK(fabsf(sample - (32767.0f / 32768.0f)) < 0.00001f);
    CHECK(vt_phone_packet_sample(&packet, packet.sample_count, &sample) == 0);

    CHECK(vt_phone_packet_parse(datagram, sizeof(datagram) - 1U, &packet) == 0);
    CHECK(packet.pcm_le == NULL);

    datagram[0] = (uint8_t)'X';
    CHECK(vt_phone_packet_parse(datagram, sizeof(datagram), &packet) == 0);
    datagram[0] = (uint8_t)'V';

    datagram[4] = 99U;
    CHECK(vt_phone_packet_parse(datagram, sizeof(datagram), &packet) == 0);
    datagram[4] = VT_PHONE_PACKET_VERSION;

    write_u16_le(datagram + 6, 19U);
    CHECK(vt_phone_packet_parse(datagram, sizeof(datagram), &packet) == 0);

    CHECK(vt_phone_packet_parse(NULL, sizeof(datagram), &packet) == 0);
    CHECK(vt_phone_packet_parse(datagram, sizeof(datagram), NULL) == 0);

    puts("All phone audio packet tests passed.");
    return 0;
}
