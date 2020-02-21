#include <Arduino.h>
#include <PacketSerial.h>
#include "serial_definitions.h"

#define MODE_FRONT 3
#define MODE_BACK 2

PacketSerial packetSerial;
PacketSerial packetSerial1;
PacketSerial packetSerial3;

void onPacketReceivedFromESP32(const uint8_t* buffer, size_t size)
{
    PacketHeader header;
    memcpy(&header, buffer, sizeof(PacketHeader));

    if(header.flag == FORWARD)
    {
        packetSerial1.send(buffer, size);
    }
}

void onPacketReceivedFromV5(const uint8_t* buffer, size_t size)
{
    PacketHeader header;
    memcpy(&header, buffer, sizeof(PacketHeader));

    if(header.flag == FORWARD)
    {
        packetSerial3.send(buffer, size);
        Serial3.write(0);
    }
}

void sendClear(uint8_t line)
{
    uint8_t sendBuffer[sizeof(PacketHeader) + sizeof(ClearPacket)];
    PacketHeader header;
    header.flag = FORWARD;
    header.type = CLEAR;

    ClearPacket clearPacket;
    clearPacket.line = line;

    memcpy(sendBuffer, &header, sizeof(PacketHeader));
    memcpy(sendBuffer + sizeof(PacketHeader), &clearPacket, sizeof(ClearPacket));

    packetSerial1.send(sendBuffer, sizeof(PacketHeader) + sizeof(ClearPacket));
}

void sendDemo(uint8_t line)
{
    PacketHeader header;
    header.flag = PARSE_MSGPACK;
    header.type = PRINT_DEMO;

    PrintDemoPacket demoPacket;
    demoPacket.line = line;

    uint8_t sendBuffer[sizeof(PacketHeader) + sizeof(PrintDemoPacket)];
    
    memcpy(sendBuffer, &header, sizeof(PacketHeader));
    memcpy(sendBuffer + sizeof(PacketHeader), &demoPacket, sizeof(PrintDemoPacket));

    packetSerial1.send(sendBuffer, sizeof(PacketHeader) + sizeof(PrintDemoPacket));
}

void sendAntiTip(float gyroY)
{
    PacketHeader header;
    header.flag = PARSE_MSGPACK;
    header.type = ANTI_TIP;

    AntiTipPacket antiTipPacket;
    antiTipPacket.gyroY = gyroY;

    uint8_t sendBuffer[sizeof(PacketHeader) + sizeof(AntiTipPacket)];
    memcpy(sendBuffer, &header, sizeof(PacketHeader));
    memcpy(sendBuffer + sizeof(PacketHeader), &antiTipPacket, sizeof(AntiTipPacket));
    packetSerial1.send(sendBuffer, sizeof(PacketHeader) + sizeof(AntiTipPacket));

}

void setupCommunication()
{
    Serial.begin(115200);
    Serial1.begin(115200);
    Serial3.begin(115200);

    packetSerial1.setStream(&Serial1);

    packetSerial3.setStream(&Serial3);
    packetSerial3.setPacketHandler(&onPacketReceivedFromESP32);

    // Set the RS485 board to transmit
    pinMode(MODE_FRONT, OUTPUT);
    digitalWrite(MODE_FRONT, HIGH);
}

void communicationLoop()
{
    packetSerial1.update();
    packetSerial3.update();
}