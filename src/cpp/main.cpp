#include <napi.h>
#include <helpers.cpp>
#include "captureWindow.cpp"
#include <getWindowData.cpp>
#include <keyboard.cpp>
#include <mouse.cpp>

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    exports.Set("getWindowData", Napi::Function::New(env, GetWindowData));
    exports.Set("captureWindow", Napi::Function::New(env, CaptureWindow));
    exports.Set("keyDownHandler", Napi::Function::New(env, SetKeyDownCallback));
    exports.Set("keyUpHandler", Napi::Function::New(env, SetKeyUpCallback));
    exports.Set("mouseMove", Napi::Function::New(env, MoveMouse));
    exports.Set("mouseClick", Napi::Function::New(env, ClickMouse));
    exports.Set("typeString", Napi::Function::New(env, TypeString));
    return exports;
}

NODE_API_MODULE(addon, Init)
