#pragma once
typedef struct {
    float x;
    float y;
} Vec2;

// Set to 1, 2, 4, 8, or 16 before including this header.
#ifndef MSAA_SAMPLES
#define MSAA_SAMPLES 4
#endif

// Sample positions within a pixel for each MSAA level.
#if MSAA_SAMPLES == 1
static const Vec2 sampleOffsets[1] = {
    {0.50f, 0.50f}
};
#elif MSAA_SAMPLES == 2
static const Vec2 sampleOffsets[2] = {
    {0.25f, 0.75f},
    {0.75f, 0.25f}
};
#elif MSAA_SAMPLES == 4
static const Vec2 sampleOffsets[4] = {
    {0.375f, 0.125f},
    {0.875f, 0.375f},
    {0.125f, 0.625f},
    {0.625f, 0.875f}
};
#elif MSAA_SAMPLES == 8
static const Vec2 sampleOffsets[8] = {
    {0.5625f, 0.3125f},
    {0.4375f, 0.6875f},
    {0.8125f, 0.5625f},
    {0.3125f, 0.1875f},
    {0.1875f, 0.8125f},
    {0.0625f, 0.4375f},
    {0.6875f, 0.9375f},
    {0.9375f, 0.0625f}
};
#elif MSAA_SAMPLES == 16
static const Vec2 sampleOffsets[16] = {
    {0.5625f, 0.5625f},
    {0.4375f, 0.3125f},
    {0.3125f, 0.6250f},
    {0.7500f, 0.4375f},
    {0.1875f, 0.3750f},
    {0.6250f, 0.8125f},
    {0.8125f, 0.1875f},
    {0.6875f, 0.6875f},
    {0.3750f, 0.8750f},
    {0.5000f, 0.0625f},
    {0.2500f, 0.1250f},
    {0.1250f, 0.7500f},
    {0.0000f, 0.5000f},
    {0.9375f, 0.2500f},
    {0.8750f, 0.9375f},
    {0.0625f, 0.0000f}
};
#else
#error "MSAA_SAMPLES must be 1, 2, 4, 8, or 16"
#endif
