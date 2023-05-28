
#include "helpers.h"

std::wstring ToWChar(const std::string &str)
{
    int bufferSize = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    std::wstring wstr(bufferSize, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], bufferSize);
    return wstr;
}

HWND GetWindowByName(const char *windowName)
{
    return FindWindowA(NULL, windowName);
}