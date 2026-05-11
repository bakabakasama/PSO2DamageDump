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

		output << "timestamp, instanceID, sourceID, sourceName, targetID, targetName, attackID, damage, IsJA, IsCrit, IsMultiHit, IsMisc, IsMisc2" << std::endl;

		pso2hRegisterHandlerRecv(getNames, 0x08, 0x04, "Get names to assign to IDs");
		pso2hRegisterHandlerRecv(getPetInfo, 0x08, 0x11, "Get Pet ID to map to owner");
		pso2hRegisterHandlerRecv(getUserInfo, 0x0F, 0x0D, "Get current player ID");
		pso2hRegisterHandlerRecv(getNames2, 0x08, 0x0D, "Get enemy names to assign to IDs");
		pso2hRegisterHandlerRecv(getDamage, 0x04, 0x52, "Get damage linked to IDs");
		pso2hRegisterHandlerRecv(getUserActionInfo, 0x04, 0x15, "Get user action info (for turret mounting)");
		pso2hRegisterHandlerRecv(getObjectInfo, 0x08, 0x10, "Get object ID to map to owner");

		CreateThread(NULL, 0, outputDamage, NULL, 0, (LPDWORD)&outputThread);

		return 0;
	}

	void getDamage(LPBYTE packet)
	{
		Packet *pkt = new Packet(&packet);
		void* info = malloc(pkt->dataSize);
		memcpy(info, pkt->data, pkt->dataSize);
		if (!PostThreadMessage(outputThread, MSG_COMBAT_ACTION, (WPARAM)info, 0))
			free(info);
		return;
	}

	void getNames(LPBYTE packet)
	{
		Packet *pkt = new Packet(&packet);
		void* info = malloc(pkt->dataSize);
		memcpy(info, pkt->data, pkt->dataSize);
		if (!PostThreadMessage(outputThread, MSG_NEW_NAME, (WPARAM)info, 0))
			free(info);
	}

	void getNames2(LPBYTE packet)
	{
		Packet *pkt = new Packet(&packet);
		void* info = malloc(pkt->dataSize);
		memcpy(info, pkt->data, pkt->dataSize);
		if (!PostThreadMessage(outputThread, MSG_NEW_NAME2, (WPARAM)info, 0))
			free(info);
	}

	void getPetInfo(LPBYTE packet)
	{
		Packet *pkt = new Packet(&packet);
		void* info = malloc(pkt->dataSize);
		memcpy(info, pkt->data, pkt->dataSize);
		if (!PostThreadMessage(outputThread, MSG_NEW_PET, (WPARAM)info, 0))
			free(info);
	}

	void getUserInfo(LPBYTE packet)
	{
		Packet *pkt = new Packet(&packet);
		void* info = malloc(pkt->dataSize);
		memcpy(info, pkt->data, pkt->dataSize);
		if (!PostThreadMessage(outputThread, MSG_YOU_SPAWN, (WPARAM)info, 0))
			free(info);
	}

	void getUserActionInfo(LPBYTE packet)
	{
		Packet *pkt = new Packet(&packet);
		void* info = malloc(pkt->dataSize);
		memcpy(info, pkt->data, pkt->dataSize);
		if (!PostThreadMessage(outputThread, MSG_USER_ACTION, (WPARAM)info, 0))
			free(info);
	}


	void getObjectInfo(LPBYTE packet)
	{
		Packet *pkt = new Packet(&packet);
		void* info = malloc(pkt->dataSize);
		memcpy(info, pkt->data, pkt->dataSize);
		if (!PostThreadMessage(outputThread, MSG_OBJECT_SPAWN, (WPARAM)info, 0))
			free(info);
	}

	static DWORD WINAPI outputDamage(LPVOID param)
	{
		UNREFERENCED_PARAMETER(param);

		while (1)
		{
			MSG msg;
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
					output << 0 << ","
						<< 0 << ","
						<< info.playerID << ","
						<< L"YOU" << ","
						<< 0 << ","
						<< 0 << ","
						<< 0 << ","
						<< 0 << ","
						<< 0 << ","
						<< 0 << ","
						<< 0 << ","
						<< 0 << ","
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

			handleDamage(info, name1, name2);

			//timestamp, instanceID, sourceID, sourceName, targetID, targetName, attackID, damage, IsJA, IsCrit, IsMultiHit, IsMisc, IsMisc2
			output << ts << ","
				<< info.instanceID << ","
				<< info.sourceID << ","
				<< name1 << ","
				<< info.targetID << ","
				<< name2 << ","
				<< info.atkID << ","
				<< info.value << ","
				<< CheckDamageFlag(info.flags, DF_JA) << ","
				<< CheckDamageFlag(info.flags, DF_CRIT) << ","
				<< CheckDamageFlag(info.flags, DF_MH) << ","
				<< CheckDamageFlag(info.flags, DF_MISC) << ","
				<< CheckDamageFlag(info.flags, DF_MISC2)
				<< std::endl;

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
