#include <napi.h>
#include <Windows.h>
#include <thread>
#include <iostream>

// Global variable to store the JavaScript callback function
Napi::ThreadSafeFunction globalJsCallbackKeyDown;
Napi::ThreadSafeFunction globalJsCallbackKeyUp;

// Global variable to store the previous key state
bool isKeyPressed = false;
bool handleKeyUp = false;
bool handleKeyDown = false;
bool monitorThreadRunning = false;
int previousKeyState;
LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
  if (nCode >= 0)
  {
    if (wParam == WM_KEYDOWN)
    {
      KBDLLHOOKSTRUCT *kbdStruct = (KBDLLHOOKSTRUCT *)lParam;
      int keyCode = kbdStruct->vkCode;

      // Handle the keyCode here or call your JavaScript callback
      // std::cout << "Key Code: " << keyCode << std::endl;

      if (keyCode != previousKeyState || !isKeyPressed)
      {
        isKeyPressed = true;
        previousKeyState = keyCode;
        if (handleKeyDown)
        {
          int *keyCodeCopy = new int(keyCode); // Create a copy of the keyCode value

          napi_status status = globalJsCallbackKeyDown.BlockingCall(
              keyCodeCopy,
              [](Napi::Env env, Napi::Function jsCallback, int *keyCodePtr)
              {
                Napi::HandleScope scope(env);
                int keyCode = *keyCodePtr;
                jsCallback.Call({Napi::Number::New(env, keyCode)});
                delete keyCodePtr; // Clean up the dynamically allocated keyCode copy
              });

          if (status != napi_ok)
          {
            // Handle the error
          }
        }
      }
    }
    else if (wParam == WM_KEYUP)
    {
      KBDLLHOOKSTRUCT *kbdStruct = (KBDLLHOOKSTRUCT *)lParam;
      int keyCode = kbdStruct->vkCode;

      if (keyCode == previousKeyState)
      {
        isKeyPressed = false;
        if (handleKeyUp)
        {
          int *keyCodeCopy = new int(keyCode); // Create a copy of the keyCode value
          napi_status status = globalJsCallbackKeyUp.BlockingCall(
              keyCodeCopy,
              [](Napi::Env env, Napi::Function jsCallback, int *keyCodePtr)
              {
                Napi::HandleScope scope(env);
                int keyCode = *keyCodePtr;
                jsCallback.Call({Napi::Number::New(env, keyCode)});
                delete keyCodePtr; // Clean up the dynamically allocated keyCode copy
              });

          if (status != napi_ok)
          {
            // Handle the error
          }
        }
      }
    }
  }

  return CallNextHookEx(NULL, nCode, wParam, lParam);
}

// Function to monitor keyboard events
void MonitorKeyboardEvents()
{
  HHOOK keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, NULL, 0);
  if (keyboardHook == NULL)
  {
    // Error setting hook
    return;
  }

  // Main loop to process keyboard events
  MSG message;
  while (GetMessage(&message, NULL, 0, 0) > 0)
  {
    TranslateMessage(&message);
    DispatchMessage(&message);
  }

  UnhookWindowsHookEx(keyboardHook);
}

// Function called from JavaScript to set the callback function
Napi::Value SetKeyDownCallback(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  // Get the callback function from the arguments
  Napi::Function jsCallback = info[0].As<Napi::Function>();

  // Create a ThreadSafeFunction with the JavaScript callback
  globalJsCallbackKeyDown = Napi::ThreadSafeFunction::New(
      env,
      jsCallback,
      "KeyDownCallback",
      0,
      1,
      [](Napi::Env)
      {
        // Finalizer callback (optional)
      });
  handleKeyDown = true;
  if (!monitorThreadRunning)
  {
    std::thread monitorThread(MonitorKeyboardEvents);
    monitorThread.detach();

    monitorThreadRunning = true;
  }

  return env.Undefined();
}

// Function called from JavaScript to set the callback function
Napi::Value SetKeyUpCallback(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  // Get the callback function from the arguments
  Napi::Function jsCallback = info[0].As<Napi::Function>();

  // Create a ThreadSafeFunction with the JavaScript callback
  globalJsCallbackKeyUp = Napi::ThreadSafeFunction::New(
      env,
      jsCallback,
      "KeyUpCallback",
      0,
      1,
      [](Napi::Env)
      {
        // Finalizer callback (optional)
      });
  handleKeyUp = true;
  if (!monitorThreadRunning)
  {
    std::thread monitorThread(MonitorKeyboardEvents);
    monitorThread.detach();
    monitorThreadRunning = true;
  }
  return env.Undefined();
}

Napi::Value TypeString(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  if (info.Length() < 1 || !info[0].IsString())
  {
    Napi::TypeError::New(env, "You should provide a string to type").ThrowAsJavaScriptException();
    return env.Null();
  }
  int delay = 15;
  std::string text = info[0].As<Napi::String>();
  if (info.Length() > 1 && info[1].IsNumber())
  {
    delay = info[1].As<Napi::Number>();
  }
  // Convert the string to wide string
  std::wstring wideText(text.begin(), text.end());

  // Simulate typing by sending keyboard events
  for (const wchar_t &character : wideText)
  {
    INPUT keyboardInput = {0};
    keyboardInput.type = INPUT_KEYBOARD;
    keyboardInput.ki.wVk = 0;
    keyboardInput.ki.wScan = character;
    keyboardInput.ki.dwFlags = KEYEVENTF_UNICODE;
    keyboardInput.ki.time = 0; // System will provide the timestamp

    SendInput(1, &keyboardInput, sizeof(keyboardInput));

    keyboardInput.ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
    SendInput(1, &keyboardInput, sizeof(keyboardInput));

    // Sleep for 15 milliseconds between each character input
    Sleep(delay);
  }

  return Napi::Boolean::New(env, true);
}