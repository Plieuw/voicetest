#include "voice_engine.h"

#include <math.h>

#define VT_YIN_MAX_TAU 2048U
#define VT_YIN_THRESHOLD 0.15f
#define VT_YIN_MAX_ACCEPTABLE_CMND 0.35f

int vt_analyse_basic(const vt_audio_frame_t *frame,
                     vt_basic_features_t *features)
{
    size_t i;
    float sum_squares = 0.0f;
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
        sum_squares += sample * sample;

        if (i > 0) {
            const float previous = frame->samples[i - 1];
            if ((previous < 0.0f && sample >= 0.0f) ||
                (previous >= 0.0f && sample < 0.0f)) {
                ++crossings;
            }
        }
    }

    features->peak = peak;
    features->rms = (float)sqrtf(sum_squares / (float)frame->sample_count);
    features->zero_crossing_rate =
        frame->sample_count > 1
            ? (float)crossings / (float)(frame->sample_count - 1)
            : 0.0f;
    features->has_samples = 1;
    return 1;
}

int vt_estimate_pitch_yin(const vt_audio_frame_t *frame,
                          float min_frequency_hz,
                          float max_frequency_hz,
                          vt_pitch_result_t *result)
{
    float difference[VT_YIN_MAX_TAU + 1U];
    float cumulative = 0.0f;
    float best_cmnd = 1.0f;
    unsigned int min_tau;
    unsigned int max_tau;
    unsigned int tau;
    unsigned int selected_tau = 0U;
    size_t usable_samples;
    float energy = 0.0f;

    if (result == NULL) {
        return 0;
    }
    result->frequency_hz = 0.0f;
    result->confidence = 0.0f;
    result->is_voiced = 0;

    if (frame == NULL || frame->samples == NULL ||
        frame->sample_rate_hz == 0U || frame->sample_count < 8U ||
        !isfinite(min_frequency_hz) || !isfinite(max_frequency_hz) ||
        min_frequency_hz <= 0.0f ||
        max_frequency_hz <= min_frequency_hz) {
        return 0;
    }

    min_tau = (unsigned int)((float)frame->sample_rate_hz / max_frequency_hz);
    max_tau = (unsigned int)((float)frame->sample_rate_hz / min_frequency_hz);
    if (min_tau < 2U) {
        min_tau = 2U;
    }
    if (max_tau > VT_YIN_MAX_TAU ||
        max_tau <= min_tau ||
        frame->sample_count <= (size_t)max_tau + 2U) {
        return 0;
    }

    usable_samples = frame->sample_count - (size_t)max_tau;
    for (tau = 0U; tau <= max_tau; ++tau) {
        difference[tau] = 0.0f;
    }

    for (tau = 1U; tau <= max_tau; ++tau) {
        float sum = 0.0f;
        size_t i;
        for (i = 0U; i < usable_samples; ++i) {
            const float delta = frame->samples[i] - frame->samples[i + tau];
            sum += delta * delta;
        }
        difference[tau] = (float)sum;
    }

    for (tau = 1U; tau <= max_tau; ++tau) {
        cumulative += difference[tau];
        difference[tau] = cumulative > 0.0f
            ? difference[tau] * (float)tau / cumulative
            : 1.0f;
    }

    for (tau = 0U; tau < frame->sample_count; ++tau) {
        const float sample = frame->samples[tau];
        energy += sample * sample;
    }
    if (energy < 1.0e-10) {
        return 1;
    }

    for (tau = min_tau; tau <= max_tau; ++tau) {
        if (difference[tau] < VT_YIN_THRESHOLD) {
            while (tau < max_tau && difference[tau + 1U] < difference[tau]) {
                ++tau;
            }
            selected_tau = tau;
            break;
        }
        if (difference[tau] < best_cmnd) {
            best_cmnd = difference[tau];
            selected_tau = tau;
        }
    }

    if (selected_tau == 0U) {
        return 1;
    }
    best_cmnd = difference[selected_tau];
    if (best_cmnd > VT_YIN_MAX_ACCEPTABLE_CMND) {
        return 1;
    }

    {
        float refined_tau = (float)selected_tau;
        if (selected_tau > min_tau && selected_tau < max_tau) {
            const float left = difference[selected_tau - 1U];
            const float centre = difference[selected_tau];
            const float right = difference[selected_tau + 1U];
            const float denominator = left - 2.0f * centre + right;
            if (fabsf(denominator) > 1.0e-9f) {
                refined_tau += 0.5f * (left - right) / denominator;
            }
        }
        if (refined_tau > 0.0f) {
            result->frequency_hz =
                (float)frame->sample_rate_hz / refined_tau;
            result->confidence = 1.0f - best_cmnd;
            result->is_voiced = 1;
        }
    }
    return 1;
}
