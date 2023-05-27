#include <napi.h>
#include <helpers.cpp>
#include "captureWindow.cpp"
#include <getWindowData.cpp>
#include <keyPress.cpp>

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    exports.Set("getWindowData", Napi::Function::New(env, GetWindowData));
    exports.Set("captureWindow", Napi::Function::New(env, CaptureWindow));
    exports.Set("keyPressHandler", Napi::Function::New(env, SetKeyPressCallback));
    return exports;
}

NODE_API_MODULE(addon, Init)
