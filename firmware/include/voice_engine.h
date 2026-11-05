#ifndef VOICETEST_VOICE_ENGINE_H
#define VOICETEST_VOICE_ENGINE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VT_YIN_MAX_TAU 1024U

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

typedef struct {
    float frequency_hz;
    float confidence;
    int is_voiced;
} vt_pitch_result_t;

/*
 * Caller-owned YIN scratch space. On the FM1, keep one instance in static
 * storage; do not put this 4 KiB workspace on the audio callback's stack.
 */
typedef struct {
    float difference[VT_YIN_MAX_TAU + 1U];
} vt_pitch_workspace_t;

int vt_analyse_basic(const vt_audio_frame_t *frame,
                     vt_basic_features_t *features);

/*
 * Estimate fundamental frequency with the YIN difference method.
 * min/max_frequency_hz bound the search; the minimum supported frequency
 * is sample_rate_hz / VT_YIN_MAX_TAU. A valid call returns 1;
 * result->is_voiced indicates whether a reliable periodic candidate exists.
 *
 * The caller supplies scratch storage so the routine does not consume a
 * large stack frame or allocate memory. One workspace must not be used by
 * concurrent calls.
 */
int vt_estimate_pitch_yin(const vt_audio_frame_t *frame,
                          float min_frequency_hz,
                          float max_frequency_hz,
                          vt_pitch_workspace_t *workspace,
                          vt_pitch_result_t *result);

#ifdef __cplusplus
}
#endif

#endif
