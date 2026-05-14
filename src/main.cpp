#include "main.h"

namespace PSO2DamageDump
{
	extern "C" BOOL APIENTRY DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
	{
		UNREFERENCED_PARAMETER(instance);
		UNREFERENCED_PARAMETER(reserved);

		switch (reason) {
		case DLL_PROCESS_ATTACH:
		{
			CreateThread(NULL, 0, initialize, NULL, 0, NULL);
			break;
		}
		case DLL_PROCESS_DETACH:
			output.flush();
			output.close();
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		}
		return TRUE;
	}

	void __cdecl getDamage(uint8_t** pktPtr)
	{
		// We got nothing or junk
		if (!pktPtr || !*pktPtr ) return;
		uint8_t* pkt = *pktPtr;

		// Something happened!
		uint16_t totalSize = *(uint16_t*)(pkt);
		// Check the size of the packet. There's a chance we get one that's been ruined by
		// some other process crashing.
		if (IsBadReadPtr(pkt, totalSize)) 
        {
            pso2hLogLine("[DamageDump-Debug] Blocked junk packet crash in getDamage");
            return;
        }
		if (totalSize < 8 || totalSize > 8192) {
        	return; 
    	}
		uint16_t dataSize = totalSize - 8;
		uint8_t* data = pkt + 8;

		// Grab the header, it holds the flags for some reason?
		uint8_t headerFlags = pkt[6];

		// Account for truncated packets
		size_t allocSize = dataSize;
    	if (allocSize < sizeof(PacketDamage))
        	allocSize = sizeof(PacketDamage);

		// Add +4 to make sure we don't chop off the null terminator
        void* info = calloc(1, allocSize + 4);
		if (info)
		{
			memcpy(info, data, dataSize);
			// Send off the flags
			((PacketDamage*)info)->flags = headerFlags;
			if (!PostThreadMessage(outputThread, MSG_COMBAT_ACTION, (WPARAM)info, 0))
			{
				pso2hLogLine("[DamageDump-Debug] getDamage PostThreadMessage Failed!");
           		free(info);
			}
			return;
		}
	}

	void __cdecl getNames(uint8_t** pktPtr)
	{
		// We got nothing or junk
		if (!pktPtr || !*pktPtr ) return;
		uint8_t* pkt = *pktPtr;

		// Something happened!
		uint16_t totalSize = *(uint16_t*)(pkt);
		if (IsBadReadPtr(pkt, totalSize)) 
        {
            pso2hLogLine("[DamageDump-Debug] Blocked junk packet crash in getNames");
            return;
        }
		if (totalSize < 8 || totalSize > 8192) {
        	return; 
    	}

		uint16_t dataSize = totalSize - 8;
		uint8_t* data = pkt + 8;

		// Account for truncated packets
		size_t allocSize = dataSize;
    	if (allocSize < sizeof(PacketCharacterInfo))
		{
        	allocSize = sizeof(PacketCharacterInfo);
		}

		// Add +4 to make sure we don't chop off the null terminator
        void* info = calloc(1, allocSize + 4);
		if (info)
		{
			memcpy(info, data, dataSize);
			if (!PostThreadMessage(outputThread, MSG_NEW_NAME, (WPARAM)info, 0))
			{
				free(info);
			}
		}
	}

	void __cdecl getNames2(uint8_t** pktPtr)
	{
		// We got nothing or junk
		if (!pktPtr || !*pktPtr ) return;
		uint8_t* pkt = *pktPtr;
		
		// Something happened!
		uint16_t totalSize = *(uint16_t*)(pkt);
		if (IsBadReadPtr(pkt, totalSize)) 
        {
            pso2hLogLine("[DamageDump-Debug] Blocked junk packet crash in getNames2");
            return;
        }
		if (totalSize < 8 || totalSize > 8192)
		{
        	return; 
    	}

		uint16_t dataSize = totalSize - 8;
		uint8_t* data = pkt + 8;

 		// Account for truncated packets
		size_t allocSize = dataSize;
    	if (allocSize < sizeof(PacketSpawnInfo))
		{
        	allocSize = sizeof(PacketSpawnInfo);
		}

		// Add +4 to make sure we don't chop off the null terminator
        void* info = calloc(1, allocSize + 4);
		if (info)
		{
			memcpy(info, data, dataSize);
			if (!PostThreadMessage(outputThread, MSG_NEW_NAME2, (WPARAM)info, 0))
				free(info);
		}
	}

	void __cdecl getPetInfo(uint8_t** pktPtr)
	{
		// We got nothing or junk
		if (!pktPtr || !*pktPtr ) return;
		uint8_t* pkt = *pktPtr;

		// Something happened!
		uint16_t totalSize = *(uint16_t*)(pkt);
		if (IsBadReadPtr(pkt, totalSize)) 
        {
            pso2hLogLine("[DamageDump-Debug] Blocked junk packet crash in getPetInfo");
            return;
        }
		if (totalSize < 8 || totalSize > 8192) {
        	return; 
    	}

		uint16_t dataSize = totalSize - 8;
		uint8_t* data = pkt + 8;

		// Account for truncated packets
		size_t allocSize = dataSize;
    	if (allocSize < sizeof(PacketPetSpawn))
		{
        	allocSize = sizeof(PacketPetSpawn);
		}

		// Add +4 to make sure we don't chop off the null terminator
        void* info = calloc(1, allocSize);
		if (info)
		{
			memcpy(info, data, dataSize);
			if (!PostThreadMessage(outputThread, MSG_NEW_PET, (WPARAM)info, 0))
				free(info);
		}
	}

	void __cdecl getUserInfo(uint8_t** pktPtr)
	{
		// We got nothing or junk
		if (!pktPtr || !*pktPtr ) return;
		uint8_t* pkt = *pktPtr;

		// Something happened!
		uint16_t totalSize = *(uint16_t*)(pkt);
		if (IsBadReadPtr(pkt, totalSize)) 
        {
            pso2hLogLine("[DamageDump-Debug] Blocked junk packet crash in getUserInfo");
            return;
        }
		if (totalSize < 8 || totalSize > 8192) {
        	return; 
    	}

		uint16_t dataSize = totalSize - 8;
		uint8_t* data = pkt + 8;

		// Account for truncated packets
		size_t allocSize = dataSize;
    	if (allocSize < sizeof(PacketUserInfo))
		{
        	allocSize = sizeof(PacketUserInfo);
		}

		// Add +4 to make sure we don't chop off the null terminator
        void* info = calloc(1, allocSize + 4);
		if (info)
		{
			memcpy(info, data, dataSize);
			if (!PostThreadMessage(outputThread, MSG_YOU_SPAWN, (WPARAM)info, 0))
				free(info);
		}
	}

	void __cdecl getUserActionInfo(uint8_t** pktPtr)
	{
		// We got nothing or junk
		if (!pktPtr || !*pktPtr ) return;
		uint8_t* pkt = *pktPtr;

		// Something happened!
		uint16_t totalSize = *(uint16_t*)(pkt);
		if (IsBadReadPtr(pkt, totalSize)) 
        {
            pso2hLogLine("[DamageDump-Debug] Blocked junk packet crash in getUserActionInfo");
            return;
        }
		if (totalSize < 8 || totalSize > 8192) {
        	return; 
    	}

		uint16_t dataSize = totalSize - 8;
		uint8_t* data = pkt + 8;

		// Account for truncated packets
		size_t allocSize = dataSize;
    	if (allocSize < sizeof(PacketPlayerAction))
		{
        	allocSize = sizeof(PacketPlayerAction);
		}

		// Add +4 to make sure we don't chop off the null terminator
        void* info = calloc(1, allocSize + 4);
		if (info)
		{
			memcpy(info, data, dataSize);
			if (!PostThreadMessage(outputThread, MSG_USER_ACTION, (WPARAM)info, 0))
				free(info);
		}
	}


	void __cdecl getObjectInfo(uint8_t** pktPtr)
	{
		// We got nothing or junk
		if (!pktPtr || !*pktPtr ) return;
		uint8_t* pkt = *pktPtr;

	 	// Something happened!
	 	uint16_t totalSize = *(uint16_t*)(pkt);
		if (IsBadReadPtr(pkt, totalSize)) 
        {
            pso2hLogLine("[DamageDump-Debug] Blocked junk packet crash in getObjectInfo");
            return;
        }
	 	if (totalSize < 8 || totalSize > 8192) {
         	return; 
     	}

	 	uint16_t dataSize = totalSize - 8;
	 	uint8_t* data = pkt + 8;
	
	 	// Account for truncated packets
	 	size_t allocSize = dataSize;
     	if (allocSize < sizeof(PacketVehicleSpawn))
		{
			allocSize = sizeof(PacketVehicleSpawn);
		}
		// Add +4 to make sure we don't chop off the null terminator
        void* info = calloc(1, allocSize + 4);
	 	if (info)
	 	{
	 		memcpy(info, data, dataSize);
	 		if (!PostThreadMessage(outputThread, MSG_OBJECT_SPAWN, (WPARAM)info, 0))
	 			free(info);
	 	}
	}

	static DWORD WINAPI initialize(LPVOID param)
	{
		time_t ts = time(NULL);
		char docspath[MAX_PATH];
		char filename[MAX_PATH];
		char delay[10 + 1];
		size_t length;

		UNREFERENCED_PARAMETER(param);

		HMODULE hPso2Host = GetModuleHandleA("pso2h.dll");
        if (hPso2Host) {
            // Set up our handles
            pso2hGetConfig = (pso2hGetConfig_t)GetProcAddress(hPso2Host, "pso2hGetConfig");
            pso2hLogLine = (pso2hLogLine_t)GetProcAddress(hPso2Host, "pso2hLogLine");
            pso2hRegisterHandlerRecv = (pso2hRegisterHandlerRecv_t)GetProcAddress(hPso2Host, "pso2hRegisterHandlerRecv");
        }

		length = pso2hGetConfig("directory", NULL, 0);
		if (length && length < sizeof(docspath))
			pso2hGetConfig("directory", docspath, sizeof(docspath));
		else
			sprintf_s(docspath, "./%s", OUTPUT_DIR);

		length = pso2hGetConfig("delay", NULL, 0);
		if (length && length < sizeof(delay)) //0 to 4294967295
		{
			pso2hGetConfig("delay", delay, sizeof(delay));
			int ret = StrToInt(delay);
			if (ret > 0)
			{
				outputDelay = ret;
				pso2hLogLine("Output delay set to %i", ret);
			}
			else
			{
				pso2hLogLine("Output delay must be greater than or equal to 0 (currently %i)", ret);
			}
		}

		CreateDirectoryA(docspath, NULL);
		sprintf_s(filename, "%s/%lli.csv", docspath, ts);

		output.imbue(std::locale(output.getloc(), new std::codecvt_utf8<wchar_t>));
		output.open(filename, std::fstream::out);

		if (!output.is_open())
			return 1;

		std::ios_base::sync_with_stdio(false);

		output << L"timestamp, instanceID, sourceID, sourceName, targetID, targetName, attackID, damage, IsJA, IsCrit, IsMultiHit, IsMisc, IsMisc2" << std::endl;

		int res = 1;
		res = pso2hRegisterHandlerRecv((void*)getNames, 0x08, 0x04, "Get names to assign to IDs");
		if (res == 0) pso2hLogLine("ERROR: Failed to register getNames hook!");
		res = pso2hRegisterHandlerRecv((void*)getPetInfo, 0x08, 0x11, "Get Pet ID to map to owner");
		if (res == 0) pso2hLogLine("ERROR: Failed to register getPetInfo hook!");
		res = pso2hRegisterHandlerRecv((void*)getUserInfo, 0x0F, 0x0D, "Get current player ID");
		if (res == 0) pso2hLogLine("ERROR: Failed to register getUserInfo hook!");
		res = pso2hRegisterHandlerRecv((void*)getNames2, 0x08, 0x0D, "Get enemy names to assign to IDs");
		if (res == 0) pso2hLogLine("ERROR: Failed to register getNames2 hook!");
		res = pso2hRegisterHandlerRecv((void*)getDamage, 0x04, 0x52, "Get damage linked to IDs");
		if (res == 0) pso2hLogLine("ERROR: Failed to register getDamage hook!");
		res = pso2hRegisterHandlerRecv((void*)getUserActionInfo, 0x04, 0x15, "Get user action info (for turret mounting)");
		if (res == 0) pso2hLogLine("ERROR: Failed to register getUserActionInfo hook!");
		res = pso2hRegisterHandlerRecv((void*)getObjectInfo, 0x08, 0x10, "Get object ID to map to owner");
		if (res == 0) pso2hLogLine("ERROR: Failed to register getObjectInfo hook!");

		CreateThread(NULL, 0, outputDamage, NULL, 0, (LPDWORD)&outputThread);

		return 0;
	}

	static DWORD WINAPI outputDamage(LPVOID param)
	{
		UNREFERENCED_PARAMETER(param);

		while (1)
		{
			// Force Windows to create the message queue for this thread immediately
        	MSG msg;
        	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);	
			
			if (!PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				Sleep(outputDelay);
				continue;
			}

			switch (msg.message)
			{
			case MSG_NEW_NAME:
			{
				PacketCharacterInfo* pInfo = (PacketCharacterInfo*)msg.wParam;
				playerNames[pInfo->playerID] = std::wstring(pInfo->name);
				petOwner.erase(pInfo->playerID);
				strReplace(playerNames[pInfo->playerID], L",", L"comma");
				strReplace(playerNames[pInfo->playerID], L"|", L"pipe");
				free((void*)msg.wParam);
				continue;
			}
			case MSG_NEW_NAME2:
			{
				PacketSpawnInfo* pInfo = (PacketSpawnInfo*)msg.wParam;
				playerNames[pInfo->ID] = converter.from_bytes(pInfo->name);
				petOwner.erase(pInfo->ID);
				strReplace(playerNames[pInfo->ID], L",", L"comma");
				strReplace(playerNames[pInfo->ID], L"|", L"pipe");
				free((void*)msg.wParam);
				continue;
			}
			case MSG_NEW_PET:
			{
				PacketPetSpawn* pInfo = (PacketPetSpawn*)msg.wParam;
				petOwner[pInfo->petID] = pInfo->ownerID;
				free((void*)msg.wParam);
				continue;
			}
			case MSG_YOU_SPAWN:
			{
				PacketUserInfo* pInfo = (PacketUserInfo*)msg.wParam;
				if (selfID != pInfo->playerID)
				{
					selfID = pInfo->playerID;
					output << 0 << L","
						<< 0 << L","
						<< pInfo->playerID << L","
						<< L"YOU" << L","
						<< 0 << L","
						<< 0 << L","
						<< 0 << L","
						<< 0 << L","
						<< 0 << L","
						<< 0 << L","
						<< 0 << L","
						<< 0 << L","
						<< 0
						<< std::endl;
				}
				free((void*)msg.wParam);
				continue;
			}
			case MSG_USER_ACTION:
			{ //Needs more testing
				PacketPlayerAction* pInfo = (PacketPlayerAction*)msg.wParam;
				if (!std::string(pInfo->action).compare("SitSuccess") && pInfo->objID && pInfo->userID)
					petOwner[pInfo->objID] = pInfo->userID;
				free((void*)msg.wParam);
				continue;
			}
			case MSG_OBJECT_SPAWN:
			{
				PacketVehicleSpawn* pInfo = (PacketVehicleSpawn*)msg.wParam;
				petOwner[pInfo->objID] = pInfo->ownerID;
				free((void*)msg.wParam);
				continue;
			}
			case MSG_COMBAT_ACTION:
				break;
			default:
				continue;
			}

			PacketDamage* pInfo = (PacketDamage*)msg.wParam;
			time_t ts = time(0);
			std::wstring name1, name2;

			pso2hLogLine("[DamageDump-Debug] Processing Damage: SourceID: %u, TargetID: %u, Damage: %d", 
                         pInfo->sourceID, pInfo->targetID, pInfo->value);

            handleDamage(pInfo, name1, name2);

            // Let's check the state of the stream BEFORE we write
            if (output.fail()) {
                pso2hLogLine("[DamageDump-Debug] ERROR: The wofstream is in a failed state BEFORE writing!");
            }

			//timestamp, instanceID, sourceID, sourceName, targetID, targetName, attackID, damage, IsJA, IsCrit, IsMultiHit, IsMisc, IsMisc2
			output << ts << L","
				<< pInfo->instanceID << L","
				<< pInfo->sourceID << L","
				<< name1 << L","
				<< pInfo->targetID << L","
				<< name2 << L","
				<< pInfo->atkID << L","
				<< pInfo->value << L","
				<< CheckDamageFlag(pInfo->flags, DF_JA) << L","
				<< CheckDamageFlag(pInfo->flags, DF_CRIT) << L","
				<< CheckDamageFlag(pInfo->flags, DF_MH) << L","
				<< CheckDamageFlag(pInfo->flags, DF_MISC) << L","
				<< CheckDamageFlag(pInfo->flags, DF_MISC2)
				<< std::endl;

			// Let's check the state of the stream AFTER we write
            if (output.fail()) {
                pso2hLogLine("[DamageDump-Debug] ERROR: The wofstream failed DURING the write! Bad string format?");
            } else {
                pso2hLogLine("[DamageDump-Debug] Success! Data flushed to CSV.");
            }

			free((void*)msg.wParam);
		}
	}

	static inline void handleDamage(PacketDamage* pInfo, std::wstring& name1, std::wstring& name2)
	{

		if (petOwner.count(pInfo->sourceID))
			pInfo->sourceID = petOwner[pInfo->sourceID];

		if (petOwner.count(pInfo->targetID))
			pInfo->targetID = petOwner[pInfo->targetID];

		if (playerNames.count(pInfo->sourceID))
			name1 = playerNames[pInfo->sourceID];
		else
			name1 = std::wstring(L"Unknown");

		if (playerNames.count(pInfo->targetID))
			name2 = playerNames[pInfo->targetID];
		else
			name2 = std::wstring(L"Unknown");
	}
}
