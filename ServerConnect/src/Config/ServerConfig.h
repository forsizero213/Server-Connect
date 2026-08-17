#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

#include <windows.h>
#include <wininet.h>
#include <string>
#include "json.hpp"

#pragma comment(lib, "wininet.lib")

using json = nlohmann::json;

struct ServerData {
    std::string ip;
    std::string port;
};

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
            catch (...) {
                // У разі помилки парсингу JSON повертаються порожні значення
            }

            InternetCloseHandle(hConnect);
        }
        InternetCloseHandle(hInternet);
    }

    return data;
}

#endif // SERVER_CONFIG_H
