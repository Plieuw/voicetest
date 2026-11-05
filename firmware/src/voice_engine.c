#include "voice_engine.h"

#include <math.h>

int vt_analyse_basic(const vt_audio_frame_t *frame,
                     vt_basic_features_t *features)
{
    size_t i;
    double sum_squares = 0.0;
    float peak = 0.0f;
    size_t crossings = 0;

    if (features == NULL) {
        return 0;
    }

    features->peak = 0.0f;
    features->rms = 0.0f;
    features->zero_crossing_rate = 0.0f;
    features->has_samples = 0;

    if (frame == NULL || frame->samples == NULL ||
        frame->sample_count == 0 || frame->sample_rate_hz == 0) {
        return 0;
    }

    for (i = 0; i < frame->sample_count; ++i) {
        const float sample = frame->samples[i];
        const float magnitude = fabsf(sample);

        if (magnitude > peak) {
            peak = magnitude;
        }

        sum_squares += (double)sample * (double)sample;

        if (i > 0) {
            const float previous = frame->samples[i - 1];
            if ((previous < 0.0f && sample >= 0.0f) ||
                (previous >= 0.0f && sample < 0.0f)) {
                ++crossings;
            }
        }
    }

    features->peak = peak;
    features->rms = (float)sqrt(sum_squares / (double)frame->sample_count);
    features->zero_crossing_rate =
        frame->sample_count > 1
            ? (float)crossings / (float)(frame->sample_count - 1)
            : 0.0f;
    features->has_samples = 1;

    return 1;
}
