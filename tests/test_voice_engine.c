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

    CHECK(vt_analyse_basic(&frame, &features) == 1);
    CHECK(features.has_samples == 1);
    CHECK(nearly_equal(features.peak, 1.0f, 0.0001f));
    CHECK(nearly_equal(features.rms, sqrtf(0.5f), 0.0001f));
    CHECK(nearly_equal(features.zero_crossing_rate, 2.0f / 3.0f, 0.0001f));

    CHECK(vt_analyse_basic(NULL, &features) == 0);
    CHECK(features.has_samples == 0);

    {
        const vt_audio_frame_t invalid = { NULL, 4, 48000 };
        CHECK(vt_analyse_basic(&invalid, &features) == 0);
    }

    puts("All voice engine tests passed.");
    return 0;
}
