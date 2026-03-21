#include "Arduino.h"
#include "SonarSensor.h"
#include "NewPing.h"

SonarSensor::SonarSensor(unsigned int trigPin, unsigned int echoPin) : trigPin(trigPin),
                                                                       echoPin(echoPin),
                                                                       sonar(trigPin, echoPin, 200) // 👈 inicializa aqui!
{
}
void SonarSensor::begin()
{
    pinMode(trigPin, OUTPUT);               // Define o Trigger como Saída
    pinMode(echoPin, INPUT);                // Define o Echo como Entrada  
}

int SonarSensor::read()
{
    unsigned long currentMillis = millis();
    if (currentMillis - lastMillis >= 30)
    {
        lastMillis = currentMillis;

        int leitura = sonar.ping_median(3);
        if (leitura > 0)
        {
            lastvalue = leitura / 58;
            return lastvalue;
        }
    }
    return lastvalue;
}
