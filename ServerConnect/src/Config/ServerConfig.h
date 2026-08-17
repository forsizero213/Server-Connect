#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

#include <windows.h>
#include <wininet.h>
#include <urlmon.h>
#include <string>
#include <vector>
#include "json.hpp"

#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "urlmon.lib")

using json = nlohmann::json;

struct ServerData {
    std::string ip;
    std::string port;
};

// Функція для завантаження файлу з URL та збереження на диск
inline bool DownloadFile(const std::string& url, const std::string& destination) {
    HRESULT hr = URLDownloadToFileA(NULL, url.c_str(), destination.c_str(), 0, NULL);
    return SUCCEEDED(hr);
}

// Функція для створення структури папок і завантаження всіх потрібних ресурсів
inline void DownloadGameResources() {
    const std::string baseUrl = "https://raw.githubusercontent.com/forsizero213/Server-Connect-files/main/";

    // Створюємо папки
    CreateDirectoryA("CustomSAA2", NULL);
    CreateDirectoryA("uiresources", NULL);
    CreateDirectoryA("uiresources\\assets", NULL);

    // Перелік файлів для завантаження та заміна у грі
    std::vector<std::pair<std::string, std::string>> filesToDownload = {
        { "CustomSAA2/loadscs.txd", "CustomSAA2/loadscs.txd" },
        { "uiresources/assets/Hud-f58999de.js", "uiresources/assets/Hud-f58999de.js" },
        { "uiresources/assets/Hud.js", "uiresources/assets/Hud.js" },
        { "uiresources/assets/radmir-logo-helloween.png", "uiresources/assets/radmir-logo-helloween.png" },
        { "uiresources/assets/radmir-logo-new-year.png", "uiresources/assets/radmir-logo-new-year.png" },
        { "uiresources/assets/radmir.js", "uiresources/assets/radmir.js" },
        { "uiresources/assets/radmir.png", "uiresources/assets/radmir.png" }
    };

    // Проходимо по всіх файлах та завантажуємо їх з GitHub
    for (const auto& file : filesToDownload) {
        std::string downloadUrl = baseUrl + file.first;
        std::string localPath = file.second;
        DownloadFile(downloadUrl, localPath);
    }
}

inline ServerData FetchServerConfig() {
    ServerData data;
    HINTERNET hInternet = InternetOpenA("ServerConnectAgent", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    
    if (hInternet) {
        HINTERNET hConnect = InternetOpenUrlA(hInternet, "https://elitex.fun/ip", NULL, 0, INTERNET_FLAG_RELOAD | INTERNET_FLAG_SECURE, 0);
        
        if (hConnect) {
            char buffer[512];
            DWORD bytesRead = 0;
            std::string response = "";

            while (InternetReadFile(hConnect, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0) {
                buffer[bytesRead] = '\0';
                response += buffer;
            }

            try {
                auto parsedJson = json::parse(response);
                if (parsedJson.is_array() && parsedJson.size() >= 2) {
                    data.ip = parsedJson[0].get<std::string>();
                    data.port = parsedJson[1].get<std::string>();
                }
            }
            catch (...) {}

            InternetCloseHandle(hConnect);
        }
        InternetCloseHandle(hInternet);
    }

    return data;
}

#endif // SERVER_CONFIG_H