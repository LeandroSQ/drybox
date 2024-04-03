#include "pid.hpp"

PID::PID(double kp, double ki, double kd, double setPoint, double outputMin, double outputMax, bool reverse = DIRECT) {
    this->kp = kp;
    this->ki = ki;
    this->kd = kd;
    this->setPoint = setPoint;
    this->output = 0.0;
    this->lastInput = 0.0;
    this->integral = 0.0;
    this->outputMin = outputMin;
    this->outputMax = outputMax;
    this->reverse = reverse;
}

void PID::reset() {
    lastInput = 0.0;
    integral = 0.0;
    output = 0.0;
}

void PID::update(double input) {
    double error = setPoint - input;
    if (reverse) {
        error = -error;
    }

    double dInput = input - lastInput;
    integral += ki * error;

    if (integral > outputMax) {
        integral = outputMax;
    } else if (integral < outputMin) {
        integral = outputMin;
    }

    output = kp * error + integral - kd * dInput;
    if (output > outputMax) {
        output = outputMax;
    } else if (output < outputMin) {
        output = outputMin;
    }

    lastInput = input;
}

void PID::update(double inputA, double weightA, double inputB, double weightB) {
    double input = inputA * weightA + inputB * weightB;
    update(input);
}

void PID::update(double inputA, double weightA, double inputB, double weightB, double inputC, double weightC) {
    double input = inputA * weightA + inputB * weightB + inputC * weightC;
    update(input);
}