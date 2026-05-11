#include <windows.h>
#include <cstdint>

#ifdef _MSC_VER
    // Microsoft Visual C++ definitions
    #define ATTRPACK
#else
    // GCC / MinGW definitions
    #define ATTRPACK __attribute__((packed))
#endif

namespace PSO2DamageDump
{
	class Packet 
    {
    public:
        DWORD dataSize;
        LPBYTE data;

        // Constructor takes LPBYTE*
        Packet(LPBYTE* rawPacketPtr);
    };

    // Function pointer definitions
    typedef size_t (__cdecl *pso2hGetConfig_t)(const char*, char*, size_t);
    typedef void   (__cdecl *pso2hLogLine_t)(const char*, ...);
	typedef int (__cdecl *pso2hRegisterHandlerRecv_t)(void* callback, uint8_t mainId, uint8_t subId, const char* handlerName);
	
    // Extern declarations
    extern pso2hGetConfig_t pso2hGetConfig;
    extern pso2hLogLine_t   pso2hLogLine;
    extern pso2hRegisterHandlerRecv_t pso2hRegisterHandlerRecv;

#pragma pack(push, 1)

	struct PacketCharacterInfo
	{
		DWORD playerID; /*0x8-0xB*/
		BYTE unk1[0x64]; /*0xc-0x6f*/
		WCHAR name[0x12]; /*0x70-0x93*/
		BYTE unk2[0x130]; /*0x94-0x1c3*/
		WCHAR title[0x24]; /*1c4-0x20b*/
		WCHAR account[0x40]; /*20c-28b*/
	} ATTRPACK;

	struct PacketUserInfo
	{
		DWORD playerID; /*0x8-0xB*/
		/*Don't care about the rest!*/
	} ATTRPACK;

	struct PacketSpawnInfo
	{
		DWORD ID; /*0x8-0xB*/
		BYTE unk1[0x18]; /*0xc-0x23*/
		CHAR name[0x1D]; /*0x24-?, not sure how long it is*/
	} ATTRPACK;
	
	struct PacketDamage
	{
		DWORD playerID; // 0
        BYTE unk1[8];   // +4
        DWORD targetID; // +12
        BYTE unk2[6];   // +16
        WORD instanceID;// +22
        DWORD sourceID; // +24
        BYTE unk3[8];   // +28
        DWORD atkID;    // +36
        INT32 value;    // +40 (Damage)
        BYTE unk4[20];  // +44
        BYTE flags;     // +64
        BYTE unk5[11];  // +65
	} ATTRPACK;

	struct PacketVehicleSpawn /*08.10*/
	{
		DWORD objID;
		BYTE unk1[0x18];
		BYTE objName[0x16];
		BYTE unk2[0x22];
		DWORD ownerID;
		BYTE unk3[0x8];
	} ATTRPACK;
	
	struct PacketPetSpawn /*08.11*/
	{
		DWORD petID;
		BYTE unk1[0x18]; /*Some random pet info, idk*/
		BYTE petName[0x16]; /*It's not a wchar actually, for once*/
		BYTE unk2[0x72];
		DWORD ownerID;
		BYTE unk3[0x20];
	} ATTRPACK;
	
	struct PacketPlayerAction /*04.15*/
	{
		DWORD playerID;
		BYTE unk1[0x8];
		DWORD objID;
		BYTE unk2[0xC];
		DWORD userID; /*The user taking the action*/
		BYTE unk3[0x1C];
		CHAR action[0xC];
	} ATTRPACK;

#pragma pack(pop)

}
