#include <Wire.h>
#include <NewPing.h>
#include "hardware/Motor/Motor.h"
#include "hardware/SonarSensor/SonarSensor.h"

#define MOTOR_1_A 30
#define MOTOR_1_B 32

#define MOTOR_2_A 34
#define MOTOR_2_B 36

#define MOTOR_PWM_1 7
#define MOTOR_PWM_2 6

#define deadZonePwm 90
#define maxLimitPwm 255

#define SENSOR_TrigPin_FL 2
#define SENSOR_EchoPin_FL 3

#define SENSOR_TrigPin_FR 9
#define SENSOR_EchoPin_FR 8

#define SENSOR_TrigPin_BL 12
#define SENSOR_EchoPin_BL 13

#define SENSOR_TrigPin_BR 10
#define SENSOR_EchoPin_BR 11

// medida em centrimetro
int distanceDetection = 50;
#define PotDistancePin A0
#define minLimitDistance 5
#define maxLimitDistance 80
unsigned long potLeftlastMillis = 0;

int currentMaxLimitPwm = 0;
#define PotPwmPin A1
unsigned long potRightlastMillis = 0;

unsigned long TimeReleasePots = 1000;

Motor Motor_A(MOTOR_1_A, MOTOR_1_B, MOTOR_PWM_1, maxLimitPwm, deadZonePwm);
Motor Motor_B(MOTOR_2_A, MOTOR_2_B, MOTOR_PWM_2, maxLimitPwm, deadZonePwm);

SonarSensor Front_Sensor_FL(SENSOR_TrigPin_FL, SENSOR_EchoPin_FL);
SonarSensor Front_Sensor_FR(SENSOR_TrigPin_FR, SENSOR_EchoPin_FR);

SonarSensor Front_Sensor_BL(SENSOR_TrigPin_BL, SENSOR_EchoPin_BL);
SonarSensor Front_Sensor_BR(SENSOR_TrigPin_BR, SENSOR_EchoPin_BR);

bool isTurn = false;
bool isBackward = false;
bool isForward = false;

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000);

  Front_Sensor_FL.begin();
  Front_Sensor_FR.begin();

  Front_Sensor_BL.begin();
  Front_Sensor_BR.begin();

  Motor_A.begin();
  Motor_B.begin();

  pinMode(PotDistancePin, INPUT);
  pinMode(PotPwmPin, INPUT);

  delay(2000);
}

void readPots()
{
  unsigned long currentMillis = millis();

  if (currentMillis - potLeftlastMillis >= TimeReleasePots)
  {
    potLeftlastMillis = currentMillis;
    int potLeft = analogRead(PotDistancePin);
    distanceDetection = map(potLeft, 0, 1000, minLimitDistance, maxLimitDistance);
  }

  if (currentMillis - potRightlastMillis >= TimeReleasePots)
  {
    potRightlastMillis = currentMillis;
    int potRight = analogRead(PotPwmPin);
    currentMaxLimitPwm = map(potRight, 0, 1000, deadZonePwm, maxLimitPwm);
  }
}

void turnLeft()
{
  Motor_A.setValue(deadZonePwm);
  Motor_B.setValue(0);
}

void turnRight()
{
  Motor_A.setValue(0);
  Motor_B.setValue(deadZonePwm);
}
void forward()
{
  int distanceFL = Front_Sensor_FL.read();
  int distanceFR = Front_Sensor_FR.read();

  int motorDistanceSensor = min(distanceFL, distanceFR);

  float speedNorm = (float)currentMaxLimitPwm / maxLimitPwm;
  int dynamicMinDistance = 10 + (speedNorm * speedNorm) * 30;
  // distância dinâmica baseada na velocidade
  int dynamicDistance = map(currentMaxLimitPwm, deadZonePwm, maxLimitPwm, minLimitDistance, distanceDetection);

  // normaliza (0 a 1)
  float distNorm = (float)motorDistanceSensor / dynamicDistance;
  distNorm = constrain(distNorm, 0.0, 1.0);

  // curva (quanto maior o expoente, mais suave no começo e mais forte no final)
  float curva = distNorm * distNorm * distNorm;

  // PWM mínimo pra não travar
  int minPwm = deadZonePwm;

  // cálculo final
  int vlPwm = minPwm + curva * (currentMaxLimitPwm - minPwm);

  // parada seca bem perto

  if (motorDistanceSensor < dynamicMinDistance)
  {
    if (motorDistanceSensor < minLimitDistance)
    {
      isBackward = true;
    }
    if (!isTurn)
    {
      isTurn = true;

      if (distanceFR < distanceFL)
      {
        turnLeft();
      }
      else
      {
        turnRight();
      }
    }
  }
  else
  {
    isTurn = false;
    isBackward = false;
    Motor_A.forward();
    Motor_B.forward();
    Motor_A.setValue(vlPwm);
    Motor_B.setValue(vlPwm);
  }
}

void backward()
{
  if (isForward)
  {
    isBackward = false;
  }

  if (isBackward)
  {
    int distanceBL = Front_Sensor_BL.read();
    int distanceBR = Front_Sensor_BR.read();
    int motorDistanceSensor = min(distanceBL, distanceBR);

    Motor_A.backward();
    Motor_B.backward();

    if (motorDistanceSensor < 20)
    {

      if (!isTurn)
      {
        isTurn = true;
        if (distanceBR < distanceBL)
        {
          turnRight();
        }
        else
        {
          turnLeft();
        }
      }
    }
    else
    {
      isTurn = false;
      Motor_A.setValue(deadZonePwm);
      Motor_B.setValue(deadZonePwm);
    }
  }
}

void driver()
{
  forward();
  backward();
}

void loop()
{
  readPots();
  driver();
}
