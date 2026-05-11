#include <windows.h>
#include <queue>
#include <unordered_map>
#include <locale>
#include <ctime>
#include <fstream>
#include <codecvt>
#include <string>
#include <shlwapi.h>
#include "packetdef.h"
#include "utility.h"

namespace PSO2DamageDump
{
	// --- Configuration ---
	constexpr const char* OUTPUT_DIR = "damagelogs";
	constexpr DWORD OUTPUT_DELAY = 1000;

	// --- Thread Messages ---
	constexpr UINT MSG_COMBAT_ACTION = WM_USER + 1;
	constexpr UINT MSG_NEW_NAME      = WM_USER + 2;
	constexpr UINT MSG_NEW_NAME2     = WM_USER + 3;
	constexpr UINT MSG_NEW_PET       = WM_USER + 4;
	constexpr UINT MSG_YOU_SPAWN     = WM_USER + 5;
	constexpr UINT MSG_USER_ACTION   = WM_USER + 6;
	constexpr UINT MSG_OBJECT_SPAWN  = WM_USER + 7;

	// --- Helper Functions ---
	inline constexpr bool CheckDamageFlag(BYTE flags, BYTE flagToCheck) 
	{
	    return (flags & flagToCheck) != 0;
	}

    static DWORD WINAPI initialize(LPVOID param);
	void getNames(LPBYTE pkt);
	void getNames2(LPBYTE pkt);
	void getPetInfo(LPBYTE pkt);
	void getDamage(LPBYTE pkt);
	void getUserInfo(LPBYTE pkt);
	void getUserActionInfo(LPBYTE pkt);
	void getObjectInfo(LPBYTE pkt);
	static inline void handleDamage(PacketDamage& info, std::wstring& name1, std::wstring& name2);
	static DWORD WINAPI outputDamage(LPVOID param);

	static std::unordered_map<DWORD, std::wstring> playerNames;
	static std::unordered_map<DWORD, DWORD> petOwner;

	static volatile DWORD selfID = 0;
	static volatile DWORD outputThread = 0;

	static volatile BOOL configYOU = 0;

	std::wofstream output;
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

	static volatile DWORD outputDelay = OUTPUT_DELAY;

	enum DamageFlags
	{
		DF_JA = 0x1, //Just Attack
		//0x2?
		DF_MISC = 0x4, //Healing, lava, etc.. all use this
		DF_DMG = 0x8, //Damage
		DF_MH = 0x10, //Multi-hit, probably. One Point and Infinite Fire have this set
		DF_MISC2 = 0x20, //Zanverse uses this
		DF_CRIT = 0x40
	};
}