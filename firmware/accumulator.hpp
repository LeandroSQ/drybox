#pragma once

#include "configuration.hpp"

struct Accumulator {
    float value;
	float samples[AVG_SAMPLES];
    unsigned char count;
    unsigned char size;

    Accumulator();

    void set(float input);
};