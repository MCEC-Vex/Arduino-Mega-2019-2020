#ifndef SERIAL_DEFINITIONS_H
#define SERIAL_DEFINITIONS_H

#include <stdint.h>

enum StreamFlag : uint8_t
{
    FORWARD,
    PARSE_RAW,
    PARSE_MSGPACK
};

enum PacketType : uint8_t
{
    CLEAR,
    PRINT_DEMO,
    ENABLE_WIFI,
    PACK_PING,
    PONG,
    ANTI_TIP,
    ULTRASONIC_SETPOS
};

enum UltrasonicGroup : uint8_t
{
    LEFT,
    RIGHT,
    BACK
};

struct PacketHeader
{
    StreamFlag flag;
    PacketType type;
};

struct ClearPacket
{
    uint8_t line;
};

struct AntiTipPacket
{
    float gyroY;
};

struct PrintDemoPacket
{
    uint8_t line;
};

struct UltrasonicPosPacket
{
    UltrasonicGroup group;
    uint8_t position;
};

#endif //SERIAL_DEFINITIONS_H