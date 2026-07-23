/*
 * Copyright (c) 2026 CanVas Dev
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <windows.h>

#include "Hook.h"
#include "ConfigManager.h"

#include "MinHook.h"

namespace ConfigMgr = CanVasDev::ServerConnect::Config::ConfigManager;

namespace CanVasDev::ServerConnect::Hook {
	void* __fastcall HookedConnect(
		void* pThis,
		void* edx,
		const char* host,
		int port,
		const char* nickname,
		const char* password) noexcept
	{
		return OriginalConnect(
			pThis,
			ConfigMgr::GConfig.host.c_str(),
			ConfigMgr::GConfig.port,
			nickname,
			password
		);
	}

	bool InstallHook() noexcept {
		ConfigMgr::LoadConfig();

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