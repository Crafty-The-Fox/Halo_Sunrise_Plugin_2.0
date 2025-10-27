//-----------------------------------------------------------------------------
//  Sunrise 2.0
//
//  Dev:
//			Byrom
// 
//  Credits:
//			craftycodie - Halo hooks and address
//			craftyfoxx - demonware title edits
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "CoreHooks.h"
#include "Utilities.h"

const char* SunriseVers = "2.1.0";

in_addr sunrise_ip = { 174, 136, 231, 17 }; //TODO: Set to WH ip
//INT sunrise_port = 8000;



HANDLE hXam;
BOOL bRunContinuous = TRUE;
BOOL bLoopHasComplete = FALSE;
BOOL bIsDevkit; // Set on plugin load. Skips doing xnotify on devkits
DWORD LastTitleId;

DWORD DJHero2 = 0x4156087F;
//DWORD Blur = asdf; (stub)

VOID Initialise()
{
	hXam = GetModuleHandle(MODULE_XAM);

	if (MountPath(MOUNT_POINT, GetMountPath()) != 0)
	{
		Sunrise_Dbg("Failed to set mount point!");
		return;
	}

	while (bRunContinuous)
	{

		DWORD TitleID = XamGetCurrentTitleId();

		if (TitleID != LastTitleId)
		{
			LastTitleId = TitleID; // Set the last title id  to the current title id so we don't loop rechecking

			if (TitleID == DJHero2) // Check for both regular and alpha/beta title ids
			{
				
				char sunrise_description[XTITLE_SERVER_MAX_SERVER_INFO_LEN] = "DJ Hero 2 - DW ATVI LSP";
				RegisterActiveServer(sunrise_ip, sunrise_port, sunrise_description);

				//Readini(); // Read the ini each time Halo is loaded to avoid having to reload the plugin

				PLDR_DATA_TABLE_ENTRY PLDR_xex = (PLDR_DATA_TABLE_ENTRY)*XexExecutableModuleHandle;

				switch (PLDR_xex->TimeDateStamp) // Detects the exact xex by timestamp. Prevents patching static addresses in the wrong xex.
				{
					//TODO: add more stuff here
				case 0x4CB86B6B: // DJ Hero 2 Retail TU1
				{
					Sunrise_Dbg("DJ Hero 2 Retail TU1 detected! Initialising hooks...");
					SetupNetDllHooks();
					XNotify(L"Warehouse Intialised!");
					break;
				}
				default:
				{
					Sunrise_Dbg("Unrecognized Xex! TimeDateStamp: 0x%X", PLDR_xex->TimeDateStamp); // Print the timestamp so we can support this xex later if required.
					SetupNetDllHooks();

					XNotify(L"Warehouse Intialised!");
					break;
				}

				}
			}
			else if (TitleID == Blur)
			{
				//TODO: per title description!
				char sunrise_description[XTITLE_SERVER_MAX_SERVER_INFO_LEN] = "AMAX - DW ACTI LSP";
				RegisterActiveServer(sunrise_ip, sunrise_port, sunrise_description);

				//Readini(); // Read the ini each time Halo is loaded to avoid having to reload the plugin

				PLDR_DATA_TABLE_ENTRY PLDR_xex = (PLDR_DATA_TABLE_ENTRY)*XexExecutableModuleHandle;

				switch (PLDR_xex->TimeDateStamp) // Detects the exact xex by timestamp. Prevents patching static addresses in the wrong xex.
				{
					//todo: FIX THIS 
				case 0x4CB86B6B: // DJ Hero 2 Retail TU1
				{
					Sunrise_Dbg("DJ Hero 2 Retail TU1 detected! Initialising hooks...");
					SetupNetDllHooks();
					XNotify(L"Warehouse Intialised!");
					break;
				}
				default:
				{
					Sunrise_Dbg("Unrecognized Xex! TimeDateStamp: 0x%X", PLDR_xex->TimeDateStamp); // Print the timestamp so we can support this xex later if required.
					SetupNetDllHooks();

					XNotify(L"Warehouse Intialised!");
					break;
				}

				}

			}
		}
		Sleep(500); // Add a slight delay to the check loop
	}
	bLoopHasComplete = TRUE;
}


BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		if (IsTrayOpen())
		{
			Sunrise_Dbg("Plugin load aborted! Disc tray is open");
			HANDLE hSunrise = hModule;
			bLoopHasComplete = TRUE; // Make sure we set this so we don't get stuck in a sleep loop when we unload (DLL_PROCESS_DETACH) in a sec.
			*(WORD*)((DWORD)hSunrise + 64) = 1;
			return FALSE;
		}

		bIsDevkit = *(DWORD*)0x8E038610 & 0x8000 ? FALSE : TRUE; // Simple devkit check
		Sunrise_Dbg("v%s loaded! Running on %s kernel", SunriseVers, bIsDevkit ? "Devkit" : "Retail");
		ThreadMe((LPTHREAD_START_ROUTINE)Initialise);

		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		bRunContinuous = FALSE; // Exit the continuous loop
		while (!bLoopHasComplete) // Wait for loop to complete
			Sleep(100);

		Sleep(500);
		Sunrise_Dbg("Unloaded!");
		break;

	}
	return TRUE;
}
