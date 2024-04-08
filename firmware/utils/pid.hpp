#pragma once

#include "configuration.hpp"

struct PID {

    enum class Direction {
        DIRECT,
        REVERSE
    };

    double kp, ki, kd;
    double setPoint;
    double output;
    double lastInput;
    double integral;
    double outputMin, outputMax;
    Direction direction;
    unsigned long lastTime;

    // Feed forward control
    double feedForwardGain;

    PID(double kp, double ki, double kd, double feedForwardGain, double setPoint, double outputMin, double outputMax, Direction direction) {
        this->kp = kp;
        this->ki = ki;
        this->kd = kd;
        this->setPoint = setPoint;
        this->output = 0.0;
        this->lastInput = 0.0;
        this->integral = 0.0;
        this->outputMin = outputMin;
        this->outputMax = outputMax;
        this->feedForwardGain = feedForwardGain;
        this->direction = direction;
        this->lastTime = millis();
    }

    void reset() {
        lastInput = 0.0;
        lastTime = millis();
        integral = 0.0;
        output = 0.0;
    }

    void update(double input, double feedForwardInput) {
        unsigned long now = millis();
        double dt = (now - lastTime) / 1000.0;
        lastTime = now;

        double error = setPoint - input;
        if (direction == Direction::REVERSE) {
            error = -error;
        }

        // Apply conditional integral control to prevent integral windup
        if (output <= outputMax && output >= outputMin) {
            integral += 0.5f * ki * error * dt;
        }

        // Apply feed forward control
        double predictedDeviation = feedForwardGain * (setPoint - feedForwardInput);

        double deltaInput = input - lastInput;
        output = kp * error + integral - kd * deltaInput + predictedDeviation;

        // Constrain the output
        if (output > outputMax) {
            output = outputMax;
        } else if (output < outputMin) {
            output = outputMin;
        }

        lastInput = input;
    }

};