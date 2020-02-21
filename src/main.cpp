#include <Arduino.h>
#include "communication.h"
#include "antitip.h"

#define ACTIVE_BUZZER 7

unsigned long lastBackwardsPacket = 0;

void setup()
{
    // Play a startup beep
    pinMode(ACTIVE_BUZZER, OUTPUT);
    tone(ACTIVE_BUZZER, 1000);
    delay(70);
    noTone(ACTIVE_BUZZER);
    delay(70);
    tone(ACTIVE_BUZZER, 1000);
    delay(70);
    noTone(ACTIVE_BUZZER);

    // Setup communication and anti-tip
    setupCommunication();
    setupAntitip();
}

void loop()
{
    communicationLoop();

    if(checkIfTipping())
    {
        // Only send the tipping packet every 100 ms
        if(millis() - lastBackwardsPacket > 100)
        {
            lastBackwardsPacket = millis();
            sendAntiTip(gyroY.getMedian());
        }
    }
    
    delayMicroseconds(250);
}