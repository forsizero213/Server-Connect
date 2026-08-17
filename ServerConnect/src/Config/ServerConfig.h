#ifndef SERVER_CONFIG_H
#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

#include <windows.h>
#include <wininet.h>
#include <string>
#include <json.hpp>

#pragma comment(lib, "wininet.lib")

using json = nlohmann::json;

struct ServerData {
    std::string ip;
    std::string port;
};

inline ServerData FetchServerConfig() {
    ServerData data = { "", "" };
    
    HINTERNET hInternet = InternetOpenA("ServerConnectClient/1.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return data;

    HINTERNET hConnect = InternetOpenUrlA(hInternet, "http://elitex.fun/ip", NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (!hConnect) {
        InternetCloseHandle(hInternet);
        return data;
    }

    std::string response;
    char buffer[1024];
    DWORD bytesRead = 0;

    while (InternetReadFile(hConnect, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        response += buffer;
    }

    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    try {
        // Парсинг масиву {"192.168.0.1", "7777"}
        auto parsedJson = json::parse(response);
        if (parsedJson.is_array() && parsedJson.size() >= 2) {
            data.ip = parsedJson[0].get<std::string>();
            data.port = parsedJson[1].get<std::string>();
        }
    } catch (...) {}

    return data;
}

#endif // SERVER_CONFIG_H
