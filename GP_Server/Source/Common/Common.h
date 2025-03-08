#pragma once

using BYTE = unsigned char;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;

constexpr uint32 MAX_CLIENT = 1000;
constexpr BYTE PKT_TYPE_INDEX = 0;
constexpr BYTE PKT_SIZE_INDEX = 1;
constexpr const char* SERVER_IP = "127.0.0.1";
constexpr int16 SERVER_PORT = 4000;
constexpr uint32 BUFSIZE = 1024;

#include "Type.h"
#include "DataStruct.h"
#include "Packet.h"