#include <Windows.h>
#include <iostream>
#include <codecvt>
#include <Windows.h>
#include <napi.h>

std::wstring ToWChar(const std::string &str);
HWND GetWindowByName(const char *windowName);
