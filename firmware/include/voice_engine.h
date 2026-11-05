#ifndef VOICETEST_VOICE_ENGINE_H
#define VOICETEST_VOICE_ENGINE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const float *samples;
    size_t sample_count;
    unsigned int sample_rate_hz;
} vt_audio_frame_t;

typedef struct {
    float peak;
    float rms;
    float zero_crossing_rate;
    int has_samples;
} vt_basic_features_t;

/*
 * Calculates basic time-domain measurements for one mono float PCM frame.
 * The function performs no allocation and does not retain the input pointer.
 * Returns 1 on success, 0 for invalid input.
 */
int vt_analyse_basic(const vt_audio_frame_t *frame,
                     vt_basic_features_t *features);

#ifdef __cplusplus
}
#endif

#endif
