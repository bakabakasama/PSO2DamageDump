#include "packetdef.h"

namespace PSO2DamageDump
{
    // Constructor
    Packet::Packet(LPBYTE* rawPacketPtr) {
        LPBYTE rawPacket = *rawPacketPtr;
        DWORD fullSize = *((DWORD*)rawPacket);
        dataSize = fullSize - 8; // Subtract 8-byte header
        data = rawPacket + 8;    // Shift pointer to payload
    }

    // Fucntion pointers!
    pso2hGetConfig_t pso2hGetConfig = nullptr;
    pso2hLogLine_t   pso2hLogLine = nullptr;
    pso2hRegisterHandlerRecv_t pso2hRegisterHandlerRecv = nullptr;
}