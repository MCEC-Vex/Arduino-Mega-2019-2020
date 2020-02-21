#include <Arduino.h>
#include <MPU6050_tockn.h>
#include <Wire.h>
#include <RunningMedian.h>
#include <limits.h>

#define ACTIVE_BUZZER 7

MPU6050 mpu6050(Wire);
RunningMedian gyroY = RunningMedian(5);
unsigned long sessionHits = 0;
unsigned long sessionMisses = 0;

void setupAntitip()
{
    Wire.begin();
    mpu6050.begin();

    // Offsets obtained from testing
    mpu6050.setGyroOffsets(-5.27, 0.77, -0.79);
}

bool checkIfTipping()
{
    mpu6050.update();

    gyroY.add(mpu6050.getGyroX() * -1);

    // If the acceleration is past a threshold, add it to the psuedo~integration
    if(gyroY.getMedian() > 20)
    {
        unsigned long med = gyroY.getMedian();
        // Ensure the long isn't overflowed (I think that's why the robot screams)
        if(sessionHits < LONG_MAX - med)
        {
            sessionHits += med;
        }
        // Output a tone proportional to the integration~ish~thing
        tone(ACTIVE_BUZZER, 200 + (sessionHits / 3));
    }
    else
    {
        // After a certain amount of acceleration under the threshold, cancel the tip session
        sessionMisses++;
        if(sessionMisses > 80)
        {
            noTone(ACTIVE_BUZZER);
            sessionMisses = 0;
            sessionHits = 0;
        }
    }

    // If the integration passes a certain amount, trigger the anti-tip
    //TODO make this timings-independent
    if(sessionHits > 6000)
    {
        return true;
    }
    return false;
}