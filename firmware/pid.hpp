#include "configuration.hpp"

struct PID {

    double kp, ki, kd;
    double setPoint;
    double output;
    double lastInput;
    double integral;
    double outputMin, outputMax;
    bool reverse;

    PID(double kp, double ki, double kd, double setPoint, double outputMin, double outputMax, bool reverse);
    void reset();
    void update(double input);
    inline void update(double inputA, double weightA, double inputB, double weightB);
    inline void update(double inputA, double weightA, double inputB, double weightB, double inputC, double weightC);

};