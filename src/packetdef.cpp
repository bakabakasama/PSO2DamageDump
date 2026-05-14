#include "packetdef.h"

namespace PSO2DamageDump
{
    // Constructor
    Packet::Packet(LPBYTE* rawPacketPtr)
    {
        LPBYTE rawPacket = *rawPacketPtr;
        if (!rawPacket) {
            dataSize = 0;
            data = nullptr;
            return;
        }

        WORD fullSize = *((WORD*)rawPacket);

        // Sanity check packet size
        if (fullSize >= 8) { 
            dataSize = fullSize - 8; 
            data = rawPacket + 8;    // Shift pointer to payload safely
        } else { 
            dataSize = 0; 
            data = nullptr;          // It's a junk/tiny packet, no payload exists
        }
    }

    // Fucntion pointers!
    pso2hGetConfig_t pso2hGetConfig = nullptr;
    pso2hLogLine_t   pso2hLogLine = nullptr;
    pso2hRegisterHandlerRecv_t pso2hRegisterHandlerRecv = nullptr;
}