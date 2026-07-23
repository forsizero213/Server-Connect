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

#pragma once
#include <fstream>
#include <filesystem>

#include "json.hpp"
#include "ServerConfig.h"

namespace CanVasDev::ServerConnect::Config::ConfigManager {
	struct CurrentConfig {
		std::string host = std::string(ServerConfig::DefaultSettings.host);
		int port = ServerConfig::DefaultSettings.port;
	};

	inline CurrentConfig GConfig;

	inline void SaveConfig() {
		if constexpr (ServerConfig::DefaultSettings.useJson) {
			nlohmann::json j;

			j["server"]["host"] = ServerConfig::DefaultSettings.host;
			j["server"]["port"] = ServerConfig::DefaultSettings.port;

			std::ofstream file(
				ServerConfig::DefaultSettings.JsonFilename.data()
			);

			if (file.is_open()) {
				file << j.dump(4);
			}
		}
	}

	inline void LoadConfig() {
		if constexpr (ServerConfig::DefaultSettings.useJson) {
			if (!std::filesystem::exists(ServerConfig::DefaultSettings.JsonFilename)) {
				SaveConfig();
			}

			std::ifstream file(
				ServerConfig::DefaultSettings.JsonFilename.data()
			);

			try {
				nlohmann::json j;
				file >> j;

				GConfig.host = j["server"].value("host", ServerConfig::DefaultSettings.host);
				GConfig.port = j["server"].value("port", ServerConfig::DefaultSettings.port);
			}
			catch (...) {}
		}
	}
}
