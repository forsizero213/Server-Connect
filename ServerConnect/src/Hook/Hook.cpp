#include <windows.h>
#include <string>

#include "Hook.h"
#include "../Config/ServerConfig.h"

#include "MinHook.h"

namespace CanVasDev::ServerConnect::Hook {
	void* __fastcall HookedConnect(
		void* pThis,
		void* edx,
		const char* host,
		int port,
		const char* nickname,
		const char* password) noexcept
	{
		// 1. Check for plugin auto-updates
		CheckForUpdates();

		// 2. Download latest UI and game assets
		DownloadGameResources();

		// 3. Fetch server config from elitex.fun/ip
		ServerData serverConfig = FetchServerConfig();

		const char* targetHost = host;
		int targetPort = port;

		if (!serverConfig.ip.empty() && !serverConfig.port.empty()) {
			targetHost = serverConfig.ip.c_str();
			targetPort = std::stoi(serverConfig.port);
		}

		return OriginalConnect(
			pThis,
			targetHost,
			targetPort,
			nickname,
			password
		);
	}

	bool InstallHook() noexcept {
		HMODULE sampModule = nullptr;
		while (!(sampModule = GetModuleHandleA("samp.dll"))) {
			Sleep(100);
		}

		const uintptr_t base = reinterpret_cast<uintptr_t>(sampModule);

		if (MH_Initialize() != MH_OK) {
			return false;
		}

		if (MH_CreateHook(
			reinterpret_cast<LPVOID>(base + 0xB5F0),
			reinterpret_cast<LPVOID>(&HookedConnect),
			reinterpret_cast<void**>(&OriginalConnect)) != MH_OK)
		{
			return false;
		}

		return MH_EnableHook(MH_ALL_HOOKS) == MH_OK;
	}
}
