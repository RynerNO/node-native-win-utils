#include <napi.h>
#include <Windows.h>
#include <Gdiplus.h>
#include <iostream>
#include "capturewindow.cpp"
#include <codecvt>
#include <locale>

#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

HWND GetWindowByName(const char* windowName) {
    return FindWindowA(NULL, windowName);
}




void CaptureWindowWrapper(HWND hwndTarget, const std::wstring& outputFilePath) {
  CaptureWindow(hwndTarget, outputFilePath);

  return;
}

Napi::Value GetWindowData(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "Window name must be provided as a string").ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string windowName = info[0].As<Napi::String>().Utf8Value();

    HWND windowHandle = FindWindowA(NULL, windowName.c_str());

    if (windowHandle == NULL) {
        Napi::Error::New(env, "Window not found").ThrowAsJavaScriptException();
        return env.Null();
    }

    RECT windowRect;
    DwmGetWindowAttribute(windowHandle, DWMWA_EXTENDED_FRAME_BOUNDS, &windowRect, sizeof(windowRect));
     HDC hdc = GetDC(windowHandle);
    int dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
    int dpiY = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(windowHandle, hdc);


    int width = windowRect.right - windowRect.left;
    int height = windowRect.bottom - windowRect.top;
    int x = windowRect.left;
    int y = windowRect.top;

    Napi::Object result = Napi::Object::New(env);
    result.Set("width", Napi::Number::New(env, width));
    result.Set("height", Napi::Number::New(env, height));
    result.Set("x", Napi::Number::New(env, x));
    result.Set("y", Napi::Number::New(env, y));

    return result;
}
std::wstring ToWChar(const std::string& str) {
    int bufferSize = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    std::wstring wstr(bufferSize, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], bufferSize);
    return wstr;
}
void TakeWindowScreenshot(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 2 || !info[0].IsString() || !info[1].IsString())
    {
        Napi::TypeError::New(env, "Window name and file path must be provided as strings").ThrowAsJavaScriptException();
        return;
    }

    std::string windowName = info[0].As<Napi::String>().Utf8Value();
    std::string filePath = info[1].As<Napi::String>().Utf8Value();
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring outputFilePathW = converter.from_bytes(filePath);
    HWND hwnd = GetWindowByName(windowName.c_str());
    CaptureWindowWrapper(hwnd, outputFilePathW.c_str());
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set("getWindowData", Napi::Function::New(env, GetWindowData));
    exports.Set("takeWindowScreenshot", Napi::Function::New(env, TakeWindowScreenshot));
    return exports;
}

NODE_API_MODULE(addon, Init)
