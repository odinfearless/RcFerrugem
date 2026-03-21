#include <Wire.h>
#include <Adafruit_GFX.h>
#include <NewPing.h>
#include "hardware/Motor/Motor.h"
#include "hardware/SonarSensor/SonarSensor.h"

#define MOTOR_1_A 30
#define MOTOR_1_B 32

#define MOTOR_2_A 34
#define MOTOR_2_B 36

#define MOTOR_PWM_1 7
#define MOTOR_PWM_2 6

#define deadZonePwm 80
#define maxLimitPwm 255

#define SENSOR_TrigPin_FL 2
#define SENSOR_EchoPin_FL 3

#define SENSOR_TrigPin_FR 9
#define SENSOR_EchoPin_FR 8

Motor Motor_A(MOTOR_1_A, MOTOR_1_B, MOTOR_PWM_1, maxLimitPwm, deadZonePwm);
Motor Motor_B(MOTOR_2_A, MOTOR_2_B, MOTOR_PWM_2, maxLimitPwm, deadZonePwm);

SonarSensor Front_Sensor_FL(SENSOR_TrigPin_FL, SENSOR_EchoPin_FL);
SonarSensor Front_Sensor_FR(SENSOR_TrigPin_FR, SENSOR_EchoPin_FR);

const int limitDistance = 60;

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000);

  Front_Sensor_FL.begin();
  Front_Sensor_FR.begin();

  Motor_A.begin();
  Motor_B.begin();
  delay(2000);
}

void readMotors()
{
  int fistanceFL = Front_Sensor_FL.read();
  int fistanceFR = Front_Sensor_FR.read();
  int motorDistanceSensor = min(fistanceFL, fistanceFR);

  int vlPwm = map(constrain(motorDistanceSensor, 0, limitDistance), 0, limitDistance, 0, maxLimitPwm);

  if (motorDistanceSensor >= limitDistance)
  {
    Motor_A.setValue(maxLimitPwm);
    Motor_B.setValue(maxLimitPwm);
  }
  else
  {
    Motor_A.setValue(vlPwm);
    Motor_B.setValue(vlPwm);
  }

  char texto[50];
  sprintf(texto, "Map - %d Leitura - %d", vlPwm, motorDistanceSensor);
  Serial.println(texto);
}

void loop()
{
  readMotors();
}
