#include <Arduino.h>
#include <MPU6050_tockn.h>
#include <Wire.h>
#include <RunningMedian.h>
#include <limits.h>

#define ACTIVE_BUZZER 7

MPU6050 mpu6050(Wire);
RunningMedian gyroY = RunningMedian(5);
float sessionHits = 0;
float sessionMisses = 0;
unsigned long lastCheckMicros = 0;

void setupAntitip()
{
    Wire.begin();
    mpu6050.begin();

    // Offsets obtained from testing
    mpu6050.setGyroOffsets(-5.27, 0.77, -0.79);
}

bool checkIfTipping()
{
    unsigned long currentCheck = micros();
    unsigned long timePassed;

    // Ensure micros hasn't overflowed (happens every 70 minutes or so)
    if(currentCheck < lastCheckMicros)
    {
        timePassed = (LONG_MAX - lastCheckMicros) + currentCheck;
    }
    else
    {
        timePassed = currentCheck - lastCheckMicros;
    }
    lastCheckMicros = currentCheck;

    mpu6050.update();

    gyroY.add(mpu6050.getGyroX() * -1);

    // If the acceleration is past a threshold, add it to the psuedo~integration
    if(gyroY.getMedian() > 20.0)
    {
        float med = gyroY.getMedian();
        
        sessionHits += (med * (timePassed / 2000.0));

        // Output a tone proportional to the integration~ish~thing
        tone(ACTIVE_BUZZER, 200 + (sessionHits / 3));
    }
    else
    {
        // After a certain amount of acceleration under the threshold, cancel the tip session
        sessionMisses += (timePassed / 2000.0);
        if(sessionMisses >= 80.0)
        {
            noTone(ACTIVE_BUZZER);
            sessionMisses = 0.0;
            sessionHits = 0.0;
        }
    }

    // If the integration passes a certain amount, trigger the anti-tip
    if(sessionHits >= 10000.0)
    {
        sessionHits = 10000.0;
        return true;
    }
    return false;
}