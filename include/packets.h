#pragma once

//Important network packet types
enum { PODAPI_HELLO,
	   PODAPI_SEVERHERE,
	   PODAPI_SAMPLE,
	   PODAPI_WALK_VEC
};
typedef unsigned char POD_Byte;
typedef int POD_Int;
typedef float POD_Float;
typedef long long POD_Time;
typedef char POD_Char;

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
	POD_Char text[128];
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

#pragma pack(pop)

#ifdef __cplusplus
} //extern "C" linkage
#endif
