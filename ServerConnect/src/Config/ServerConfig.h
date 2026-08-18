#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

#include <windows.h>
#include <wininet.h>
#include <urlmon.h>
#include <string>
#include <vector>
#include <fstream>
#include "json.hpp"

#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "urlmon.lib")

using json = nlohmann::json;

// Current plugin version
const std::string CURRENT_VERSION = "0.0.1";

struct ServerData {
    std::string ip;
    std::string port;
};

// Download a single file using URLDownloadToFileA
inline bool DownloadFile(const std::string& url, const std::string& destination) {
    HRESULT hr = URLDownloadToFileA(NULL, url.c_str(), destination.c_str(), 0, NULL);
    return SUCCEEDED(hr);
}

// Fetch HTTP body as std::string
inline std::string FetchUrlString(const std::string& url) {
    std::string response = "";
    HINTERNET hInternet = InternetOpenA("ServerConnectAgent", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (hInternet) {
        HINTERNET hConnect = InternetOpenUrlA(hInternet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD | INTERNET_FLAG_SECURE, 0);
        if (hConnect) {
            char buffer[512];
            DWORD bytesRead = 0;
            while (InternetReadFile(hConnect, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0) {
                buffer[bytesRead] = '\0';
                response += buffer;
            }
            InternetCloseHandle(hConnect);
        }
        InternetCloseHandle(hInternet);
    }
    return response;
}

// Auto-updater function
inline void CheckForUpdates() {
    std::string versionUrl = "https://raw.githubusercontent.com/forsizero213/Server-Connect-files/refs/heads/main/version";
    std::string response = FetchUrlString(versionUrl);

    if (response.empty()) return;

    try {
        auto parsedJson = json::parse(response);
        if (parsedJson.is_array() && parsedJson.size() >= 2) {
            std::string latestVersion = parsedJson[0].get<std::string>();
            std::string downloadUrl = parsedJson[1].get<std::string>();

            // If a new version is available
            if (latestVersion != CURRENT_VERSION) {
                std::string tempFile = "ServerConnect.asi.new";
                
                if (DownloadFile(downloadUrl, tempFile)) {
                    // Create a updater batch script to replace the ASI when executable unlocks/restarts
                    std::ofstream updater("update.bat");
                    if (updater.is_open()) {
                        updater << "@echo off\n";
                        updater << "timeout /t 2 /nobreak > nul\n";
                        updater << "move /y \"ServerConnect.asi.new\" \"ServerConnect.asi\"\n";
                        updater << "del \"%~f0\"\n";
                        updater.close();

                        // Launch batch script in background
                        ShellExecuteA(NULL, "open", "update.bat", NULL, NULL, SW_HIDE);
                    }
                }
            }
        }
    }
    catch (...) {}
}

// Download assets
inline void DownloadGameResources() {
    const std::string baseUrl = "https://raw.githubusercontent.com/forsizero213/Server-Connect-files/main/";

    CreateDirectoryA("CustomSAA2", NULL);
    CreateDirectoryA("uiresources", NULL);
    CreateDirectoryA("uiresources\\assets", NULL);

    std::vector<std::pair<std::string, std::string>> filesToDownload = {
        { "CustomSAA2/loadscs.txd", "CustomSAA2/loadscs.txd" },
        { "uiresources/assets/Hud-f58999de.js", "uiresources/assets/Hud-f58999de.js" },
        { "uiresources/assets/Hud.js", "uiresources/assets/Hud.js" },
        { "uiresources/assets/radmir-logo-helloween.png", "uiresources/assets/radmir-logo-helloween.png" },
        { "uiresources/assets/radmir-logo-new-year.png", "uiresources/assets/radmir-logo-new-year.png" },
        { "uiresources/assets/radmir.js", "uiresources/assets/radmir.js" },
        { "uiresources/assets/radmir.png", "uiresources/assets/radmir.png" }
    };

    for (const auto& file : filesToDownload) {
        DownloadFile(baseUrl + file.first, file.second);
    }
}

// Fetch server IP and port
inline ServerData FetchServerConfig() {
    ServerData data;
    std::string response = FetchUrlString("https://elitex.fun/ip");

    if (!response.empty()) {
        try {
            auto parsedJson = json::parse(response);
            if (parsedJson.is_array() && parsedJson.size() >= 2) {
                data.ip = parsedJson[0].get<std::string>();
                data.port = parsedJson[1].get<std::string>();
            }
        }
        catch (...) {}
    }

    return data;
}

#endif // SERVER_CONFIG_H
