# ServerConnect

![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![Platform](https://img.shields.io/badge/Platform-Windows-success)
![SA-MP](https://img.shields.io/badge/SA--MP-0.3.7%20R3-orange)
![License](https://img.shields.io/badge/License-Apache%202.0-green)

A C++ library that intercepts the SA:MP 0.3.7 R3 connection routine and overrides the target host and port before execution — designed for projects like Arizona RP, Radmir RP, Rodina RP, and Amazing RP

Built on MinHook with optional JSON configuration support.

> Designed specifically for SA:MP 0.3.7 R3 (other versions may require updated offsets).

---

## Features

- SA:MP connection routine hooking (0.3.7 R3)
- Host/port override for projects like Arizona RP, Radmir RP, Rodina RP, and Amazing RP
- Compile-time configuration
- Optional JSON runtime configuration

---

## Requirements

- Visual Studio 2022
- C++17+
- SA:MP 0.3.7 R3
- MinHook
- nlohmann/json (optional)

---

## Configuration

`Settings.h`

```cpp
#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_PORT 7777

#define JSON_FILENAME "ServerConnect.json"
#define USE_JSON_CONFIG false
```

### JSON config (optional)

```json
{
    "server": {
        "host": "127.0.0.1",
        "port": 7777
    }
}
```

---

## How it works

1. Wait for samp.dll
2. Initialize MinHook
3. Install hook on connection routine
4. Override host/port arguments
5. Call original function

---

## Usage

```cpp
if (!CanVasDev::ServerConnect::Hook::InstallHook())
{
    // failed
}
```

---

## Configuration priority

```
ServerConnect.json (if enabled)
        ?
Settings.h (fallback)
```

---

## Dependencies

- MinHook
- nlohmann/json (optional)

---

## License

Apache License 2.0 © 2026 CanVas Dev