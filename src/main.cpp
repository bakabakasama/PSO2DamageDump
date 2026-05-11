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

	void __cdecl getDamage(void* context, uint8_t* packetData, uint32_t flags, uint32_t payloadSize)
	{
		// Something happened!
		pso2hLogLine("[DamageDump-Debug] getDamage triggered! Payload Size: %u", payloadSize);

		Packet pkt(&packetData); 
        void* info = malloc(pkt.dataSize);
        memcpy(info, pkt.data, pkt.dataSize);
		if (!PostThreadMessage(outputThread, MSG_COMBAT_ACTION, (WPARAM)info, 0)) {
            pso2hLogLine("[DamageDump-Debug] PostThreadMessage FAILED!");
            free(info);
        }
		return;
	}

	void __cdecl getNames(void* context, uint8_t* packetData, uint32_t flags, uint32_t payloadSize)
	{
		// Something happened!
		pso2hLogLine("[DamageDump-Debug] getNames triggered! Payload Size: %u", payloadSize);

		Packet pkt(&packetData); 
        void* info = malloc(pkt.dataSize);
        memcpy(info, pkt.data, pkt.dataSize);
		if (!PostThreadMessage(outputThread, MSG_NEW_NAME, (WPARAM)info, 0))
			free(info);
	}

	void __cdecl getNames2(void* context, uint8_t* packetData, uint32_t flags, uint32_t payloadSize)
	{
		// Something happened!
		pso2hLogLine("[DamageDump-Debug] getNames2 triggered! Payload Size: %u", payloadSize);

		Packet pkt(&packetData); 
        void* info = malloc(pkt.dataSize);
        memcpy(info, pkt.data, pkt.dataSize);
		if (!PostThreadMessage(outputThread, MSG_NEW_NAME2, (WPARAM)info, 0))
			free(info);
	}

	void __cdecl getPetInfo(void* context, uint8_t* packetData, uint32_t flags, uint32_t payloadSize)
	{
		// Something happened!
		pso2hLogLine("[DamageDump-Debug] getPetInfo triggered! Payload Size: %u", payloadSize);

		Packet pkt(&packetData); 
        void* info = malloc(pkt.dataSize);
        memcpy(info, pkt.data, pkt.dataSize);
		if (!PostThreadMessage(outputThread, MSG_NEW_PET, (WPARAM)info, 0))
			free(info);
	}

	void __cdecl getUserInfo(void* context, uint8_t* packetData, uint32_t flags, uint32_t payloadSize)
	{
		// Something happened!
		pso2hLogLine("[DamageDump-Debug] getUserInfo triggered! Payload Size: %u", payloadSize);

		Packet pkt(&packetData); 
        void* info = malloc(pkt.dataSize);
        memcpy(info, pkt.data, pkt.dataSize);
		if (!PostThreadMessage(outputThread, MSG_YOU_SPAWN, (WPARAM)info, 0))
			free(info);
	}

	void __cdecl getUserActionInfo(void* context, uint8_t* packetData, uint32_t flags, uint32_t payloadSize)
	{
		// Something happened!
		pso2hLogLine("[DamageDump-Debug] getUserActionInfo triggered! Payload Size: %u", payloadSize);

		Packet pkt(&packetData); 
        void* info = malloc(pkt.dataSize);
        memcpy(info, pkt.data, pkt.dataSize);
		if (!PostThreadMessage(outputThread, MSG_USER_ACTION, (WPARAM)info, 0))
			free(info);
	}


	void __cdecl getObjectInfo(void* context, uint8_t* packetData, uint32_t flags, uint32_t payloadSize)
	{
		// Something happened!
		pso2hLogLine("[DamageDump-Debug] getObjectInfo triggered! Payload Size: %u", payloadSize);
		
		Packet pkt(&packetData); 
        void* info = malloc(pkt.dataSize);
        memcpy(info, pkt.data, pkt.dataSize);
		if (!PostThreadMessage(outputThread, MSG_OBJECT_SPAWN, (WPARAM)info, 0))
			free(info);
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
				PacketCharacterInfo info = *(PacketCharacterInfo*)msg.wParam;
				playerNames[info.playerID] = std::wstring(info.name);
				petOwner.erase(info.playerID);
				strReplace(playerNames[info.playerID], L",", L"comma");
				strReplace(playerNames[info.playerID], L"|", L"pipe");
				free((void*)msg.wParam);
				continue;
			}
			case MSG_NEW_NAME2:
			{
				PacketSpawnInfo info = *(PacketSpawnInfo*)msg.wParam;
				playerNames[info.ID] = converter.from_bytes(info.name);
				petOwner.erase(info.ID);
				strReplace(playerNames[info.ID], L",", L"comma");
				strReplace(playerNames[info.ID], L"|", L"pipe");
				free((void*)msg.wParam);
				continue;
			}
			case MSG_NEW_PET:
			{
				PacketPetSpawn info = *(PacketPetSpawn*)msg.wParam;
				petOwner[info.petID] = info.ownerID;
				free((void*)msg.wParam);
				continue;
			}
			case MSG_YOU_SPAWN:
			{
				PacketUserInfo info = *(PacketUserInfo*)msg.wParam;
				if (selfID != info.playerID)
				{
					selfID = info.playerID;
					output << 0 << L","
						<< 0 << L","
						<< info.playerID << L","
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
				PacketPlayerAction info = *(PacketPlayerAction*)msg.wParam;
				if (!std::string(info.action).compare("SitSuccess") && info.objID && info.userID)
					petOwner[info.objID] = info.userID;
				free((void*)msg.wParam);
				continue;
			}
			case MSG_OBJECT_SPAWN:
			{
				PacketVehicleSpawn info = *(PacketVehicleSpawn*)msg.wParam;
				petOwner[info.objID] = info.ownerID;
				free((void*)msg.wParam);
				continue;
			}
			case MSG_COMBAT_ACTION:
				break;
			default:
				continue;
			}

			PacketDamage info = *(PacketDamage*)msg.wParam;
			time_t ts = time(0);
			std::wstring name1, name2;

			pso2hLogLine("[DamageDump-Debug] Processing Damage: SourceID: %u, TargetID: %u, Damage: %d", 
                         info.sourceID, info.targetID, info.value);

            handleDamage(info, name1, name2);

            // Let's check the state of the stream BEFORE we write
            if (output.fail()) {
                pso2hLogLine("[DamageDump-Debug] ERROR: The wofstream is in a failed state BEFORE writing!");
            }

			//timestamp, instanceID, sourceID, sourceName, targetID, targetName, attackID, damage, IsJA, IsCrit, IsMultiHit, IsMisc, IsMisc2
			output << ts << L","
				<< info.instanceID << L","
				<< info.sourceID << L","
				<< name1 << L","
				<< info.targetID << L","
				<< name2 << L","
				<< info.atkID << L","
				<< info.value << L","
				<< CheckDamageFlag(info.flags, DF_JA) << L","
				<< CheckDamageFlag(info.flags, DF_CRIT) << L","
				<< CheckDamageFlag(info.flags, DF_MH) << L","
				<< CheckDamageFlag(info.flags, DF_MISC) << L","
				<< CheckDamageFlag(info.flags, DF_MISC2)
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

	static inline void handleDamage(PacketDamage& info, std::wstring& name1, std::wstring& name2)
	{

		if (petOwner.count(info.sourceID))
			info.sourceID = petOwner[info.sourceID];

		if (petOwner.count(info.targetID))
			info.targetID = petOwner[info.targetID];

		if (playerNames.count(info.sourceID))
			name1 = playerNames[info.sourceID];
		else
			name1 = std::wstring(L"Unknown");

		if (playerNames.count(info.targetID))
			name2 = playerNames[info.targetID];
		else
			name2 = std::wstring(L"Unknown");
	}
}
