#include "phone_audio_packet.h"

#include <stddef.h>
#include <stdint.h>

static uint16_t read_u16_le(const uint8_t *p)
{
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t read_u32_le(const uint8_t *p)
{
    return (uint32_t)p[0]
        | ((uint32_t)p[1] << 8)
        | ((uint32_t)p[2] << 16)
        | ((uint32_t)p[3] << 24);
}

static uint64_t read_u64_le(const uint8_t *p)
{
    return (uint64_t)read_u32_le(p)
        | ((uint64_t)read_u32_le(p + 4) << 32);
}

int vt_phone_packet_parse(const uint8_t *datagram,
                          size_t datagram_bytes,
                          vt_phone_packet_t *packet)
{
    if (packet == NULL) {
        return 0;
    }

    packet->sequence = 0U;
    packet->first_sample = 0U;
    packet->pcm_le = NULL;
    packet->sample_count = 0U;

    if (datagram == NULL || datagram_bytes != VT_PHONE_DATAGRAM_BYTES) {
        return 0;
    }

    if (datagram[0] != (uint8_t)'V' ||
        datagram[1] != (uint8_t)'T' ||
        datagram[2] != (uint8_t)'P' ||
        datagram[3] != (uint8_t)'1' ||
        datagram[4] != VT_PHONE_PACKET_VERSION ||
        datagram[5] != 0U ||
        read_u16_le(datagram + 6) != VT_PHONE_HEADER_BYTES) {
        return 0;
    }

    packet->sequence = read_u32_le(datagram + 8);
    packet->first_sample = read_u64_le(datagram + 12);
    packet->pcm_le = datagram + VT_PHONE_HEADER_BYTES;
    packet->sample_count = VT_PHONE_SAMPLES_PER_PACKET;
    return 1;
}

int vt_phone_packet_sample(const vt_phone_packet_t *packet,
                           size_t index,
                           float *sample)
{
    uint16_t raw;
    int32_t signed_value;

    if (packet == NULL || sample == NULL || packet->pcm_le == NULL ||
        index >= packet->sample_count) {
        return 0;
    }

    raw = read_u16_le(packet->pcm_le + index * 2U);
    signed_value = raw >= 0x8000U ? (int32_t)raw - 65536 : (int32_t)raw;
    *sample = (float)signed_value / 32768.0f;
    return 1;
}
