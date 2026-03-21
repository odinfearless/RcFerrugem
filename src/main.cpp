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

#define SENSOR_TrigPin_F 2
#define SENSOR_EchoPin_F 3

#define SENSOR_TrigPin_B 9
#define SENSOR_EchoPin_B 8
#define minLimitPwm 80
#define maxLimitPwm 255

Motor Motor_A(MOTOR_1_A, MOTOR_1_B, MOTOR_PWM_1, minLimitPwm, maxLimitPwm);
Motor Motor_B(MOTOR_2_A, MOTOR_2_B, MOTOR_PWM_2, minLimitPwm, maxLimitPwm);

SonarSensor Front_Sensor(SENSOR_TrigPin_F, SENSOR_EchoPin_F);
SonarSensor Back_Sensor(SENSOR_TrigPin_B, SENSOR_EchoPin_B);

const int limitDistance = 30;

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000);

  Front_Sensor.begin();
  Back_Sensor.begin();
  Motor_A.begin();
  Motor_B.begin();
  delay(2000);
}

void readMotors()
{
  int motorDistanceSensor = Front_Sensor.read();
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
