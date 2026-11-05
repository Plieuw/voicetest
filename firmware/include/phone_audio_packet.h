#ifndef VOICETEST_PHONE_AUDIO_PACKET_H
#define VOICETEST_PHONE_AUDIO_PACKET_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VT_PHONE_UDP_PORT 39000U
#define VT_PHONE_PACKET_MAGIC "VTP1"
#define VT_PHONE_PACKET_VERSION 1U
#define VT_PHONE_HEADER_BYTES 20U
#define VT_PHONE_SAMPLE_RATE_HZ 48000U
#define VT_PHONE_SAMPLES_PER_PACKET 960U
#define VT_PHONE_PCM_BYTES (VT_PHONE_SAMPLES_PER_PACKET * 2U)
#define VT_PHONE_DATAGRAM_BYTES (VT_PHONE_HEADER_BYTES + VT_PHONE_PCM_BYTES)

typedef struct {
    uint32_t sequence;
    uint64_t first_sample;
    const uint8_t *pcm_le;
    size_t sample_count;
} vt_phone_packet_t;

/*
 * Validate and decode the fixed VTP1/48 kHz/20 ms datagram header.
 * The returned PCM pointer refers to the caller-owned datagram buffer.
 * No allocation or copying is performed.
 */
int vt_phone_packet_parse(const uint8_t *datagram,
                          size_t datagram_bytes,
                          vt_phone_packet_t *packet);

/* Convert one little-endian signed PCM16 sample to float in [-1, 1). */
int vt_phone_packet_sample(const vt_phone_packet_t *packet,
                           size_t index,
                           float *sample);

#ifdef __cplusplus
}
#endif

#endif
