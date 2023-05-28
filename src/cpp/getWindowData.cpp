#include <napi.h>
#include <Windows.h>
#include <iostream>
#include <dwmapi.h>
#include <helpers.h>

Napi::Value GetWindowData(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "Window name must be provided as a string").ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string windowName = info[0].As<Napi::String>().Utf8Value();

    HWND windowHandle = GetWindowByName(windowName.c_str());

    if (windowHandle == NULL)
    {
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