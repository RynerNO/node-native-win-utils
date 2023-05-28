#include <napi.h>
#include <helpers.cpp>
#include "captureWindow.cpp"
#include <getWindowData.cpp>
#include <keyBoardHandler.cpp>

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    exports.Set("getWindowData", Napi::Function::New(env, GetWindowData));
    exports.Set("captureWindow", Napi::Function::New(env, CaptureWindow));
    exports.Set("keyDownHandler", Napi::Function::New(env, SetKeyDownCallback));
    exports.Set("keyUpHandler", Napi::Function::New(env, SetKeyUpCallback));
    return exports;
}

NODE_API_MODULE(addon, Init)
