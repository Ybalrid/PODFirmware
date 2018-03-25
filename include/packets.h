#pragma once

#include <stdint.h>

//Important network packet types
enum {
	PODAPI_HELLO,
	PODAPI_SEVERHERE,
	PODAPI_SAMPLE,
	PODAPI_WALK_VEC,
	PODAPI_SERVER_ASK
};

typedef uint8_t POD_Byte;
typedef int32_t POD_Int;
typedef float POD_Float;
typedef int64_t POD_Time;
typedef int8_t POD_Char;

#define POD_PROTOCOL_PORT 4242

#ifdef __cplusplus
extern "C" {
#endif

//Remove any padding in the structures
#pragma pack(push, 1)

//Network packet structures
struct POD_message
{
	POD_Byte packetType;
	POD_Char text[127];
};

struct POD_ServerOk
{
	POD_Byte packetType;
};

struct POD_Sample
{
	POD_Byte packetType;
	POD_Time timecode;
	POD_Int posX, posY;
	POD_Float accX, accY, accZ;
};

struct POD_WalkVector
{
	POD_Byte packetType;
	POD_Time timepoint;
	POD_Float x, y;
};

struct POD_ServerAsk
{
	POD_Byte packetType;
};

#pragma pack(pop)

#ifdef __cplusplus
} //extern "C" linkage
#endif
