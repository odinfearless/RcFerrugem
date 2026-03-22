#include "Arduino.h"
#include "Motor.h"

Motor::Motor(unsigned int pinA, unsigned int pinB, unsigned int pinPwm, unsigned int maxLimitPwm, unsigned int pwmDeadZone)
    : pinA(pinA), pinB(pinB), pinPwm(pinPwm), maxLimitPwm(maxLimitPwm), pwmDeadZone(pwmDeadZone)
{
}
void Motor::begin()
{
    pinMode(pinA, OUTPUT);
    pinMode(pinB, OUTPUT);
    pinMode(pinPwm, OUTPUT);
    forward();
}
void Motor::forward()
{
    digitalWrite(pinA, LOW);
    digitalWrite(pinB, HIGH);
}
void Motor::backward()
{
    digitalWrite(pinA, HIGH);
    digitalWrite(pinB, LOW);
}

void Motor::setValue(int val)
{
    val = val < pwmDeadZone ? 0 : constrain(val, pwmDeadZone, maxLimitPwm);
    analogWrite(pinPwm, val);
}
unsigned int Motor::getPinA()
{
    return pinA;
}
unsigned int Motor::getPinB()
{
    return pinB;
}
unsigned int Motor::getPinPwm()
{
    return pinPwm;
}
