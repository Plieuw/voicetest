#include "voice_engine.h"

#include <math.h>
#include <stdio.h>

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "FAIL: %s (line %d)\n", #condition, __LINE__); \
        return 1; \
    } \
} while (0)

static int nearly_equal(float a, float b, float tolerance)
{
    return fabsf(a - b) <= tolerance;
}

int main(void)
{
    const float samples[] = { -1.0f, 0.0f, 1.0f, 0.0f };
    const vt_audio_frame_t frame = { samples, 4, 48000 };
    vt_basic_features_t features;
    float tone[4096];
    float silence[4096] = { 0.0f };
    vt_audio_frame_t tone_frame = { tone, 4096, 48000 };
    vt_audio_frame_t silence_frame = { silence, 4096, 48000 };
    vt_pitch_workspace_t workspace;
    vt_pitch_result_t pitch;
    size_t i;

    CHECK(vt_analyse_basic(&frame, &features) == 1);
    CHECK(features.has_samples == 1);
    CHECK(nearly_equal(features.peak, 1.0f, 0.0001f));
    CHECK(nearly_equal(features.rms, sqrtf(0.5f), 0.0001f));
    CHECK(nearly_equal(features.zero_crossing_rate, 1.0f / 3.0f, 0.0001f));

    CHECK(vt_analyse_basic(NULL, &features) == 0);
    CHECK(features.has_samples == 0);
    {
        const vt_audio_frame_t invalid = { NULL, 4, 48000 };
        CHECK(vt_analyse_basic(&invalid, &features) == 0);
    }

    for (i = 0; i < 4096; ++i) {
        tone[i] = 0.8f * sinf(2.0f * 3.14159265358979323846f *
                              220.0f * (float)i / 48000.0f);
    }

    CHECK(vt_estimate_pitch_yin(&tone_frame, 70.0f, 1000.0f,
                                &workspace, &pitch) == 1);
    CHECK(pitch.is_voiced == 1);
    CHECK(nearly_equal(pitch.frequency_hz, 220.0f, 1.5f));
    CHECK(pitch.confidence > 0.8f);

    CHECK(vt_estimate_pitch_yin(&silence_frame, 70.0f, 1000.0f,
                                &workspace, &pitch) == 1);
    CHECK(pitch.is_voiced == 0);
    CHECK(vt_estimate_pitch_yin(&tone_frame, 0.0f, 1000.0f,
                                &workspace, &pitch) == 0);

    puts("All voice engine tests passed.");
    return 0;
}
