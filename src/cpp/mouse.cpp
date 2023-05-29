// patrially used code from https://github.com/octalmage/robotjs witch is under MIT License Copyright (c) 2014 Jason Stallings
#include <napi.h>
#include <windows.h>
/**
 * Move the mouse to a specific point.
 * @param point The coordinates to move the mouse to (x, y).
 */

Napi::Value MoveMouse(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 2 || !info[0].IsNumber() || !info[1].IsNumber())
    {
        Napi::TypeError::New(env, "You should provide x and y position of type number").ThrowAsJavaScriptException();
        return env.Null();
    }

    int posX = info[0].As<Napi::Number>();
    int posY = info[1].As<Napi::Number>();

    // Get the screen metrics
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Convert coordinates to absolute values
    int absoluteX = static_cast<int>((65536 * posX) / screenWidth);
    int absoluteY = static_cast<int>((65536 * posY) / screenHeight);

    // Move the mouse
    INPUT mouseInput = {0};
    mouseInput.type = INPUT_MOUSE;
    mouseInput.mi.dx = absoluteX;
    mouseInput.mi.dy = absoluteY;
    mouseInput.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | MOUSEEVENTF_VIRTUALDESK;
    mouseInput.mi.time = 0; // System will provide the timestamp

    SendInput(1, &mouseInput, sizeof(mouseInput));

    return Napi::Boolean::New(env, true);
}

Napi::Value ClickMouse(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    std::string button;

    if (info.Length() < 1 || !info[0].IsString())
        button = "left";
    else
        button = info[0].As<Napi::String>();

    WORD mouseEvent = 0;

    if (button == "left")
    {
        mouseEvent = MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP;
    }
    else if (button == "right")
    {
        mouseEvent = MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP;
    }
    else if (button == "middle")
    {
        mouseEvent = MOUSEEVENTF_MIDDLEDOWN | MOUSEEVENTF_MIDDLEUP;
    }
    else
    {
        Napi::TypeError::New(env, "Invalid button name").ThrowAsJavaScriptException();
        return env.Null();
    }

    // Perform the mouse click
    INPUT mouseInput = {0};
    mouseInput.type = INPUT_MOUSE;
    mouseInput.mi.dwFlags = mouseEvent;
    mouseInput.mi.time = 0; // System will provide the timestamp

    SendInput(1, &mouseInput, sizeof(mouseInput));

    return Napi::Boolean::New(env, true);
}