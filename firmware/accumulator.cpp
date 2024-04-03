#include "accumulator.hpp"

Accumulator::Accumulator() {
    value = 0.0f;
    count = 0;
    size = 0;
    for (unsigned char i = 0; i < AVG_SAMPLES; i++) {
        samples[i] = 0.0f;
    }
}

void Accumulator::set(float input) {
    if (count >= AVG_SAMPLES) {
        count = 0;
    }

    samples[count++] = input;
    size = count > AVG_SAMPLES ? AVG_SAMPLES : count;
    value = 0.0f;
    for (unsigned char i = 0; i < size; i++) {
        value += samples[i];
    }
    value /= (float) size;
}
