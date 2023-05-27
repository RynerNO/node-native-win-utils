#include <napi.h>
#include <Windows.h>
#include <thread>
#include <iostream>
// Global variable to store the JavaScript callback function
Napi::ThreadSafeFunction globalJsCallback;

// Global variable to store the previous key state
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
      if (int(keyCode) == previousKeyState)
        return CallNextHookEx(NULL, nCode, wParam, lParam);
      ;

      int *keyCodeCopy = new int(keyCode); // Create a copy of the keyCode value

      napi_status status = globalJsCallback.BlockingCall(
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
      previousKeyState = keyCode;
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
Napi::Value SetKeyPressCallback(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  // Get the callback function from the arguments
  Napi::Function jsCallback = info[0].As<Napi::Function>();

  // Create a ThreadSafeFunction with the JavaScript callback
  globalJsCallback = Napi::ThreadSafeFunction::New(
      env,
      jsCallback,
      "KeyPressCallback",
      0,
      1,
      [](Napi::Env)
      {
        // Finalizer callback (optional)
      });

  std::thread monitorThread(MonitorKeyboardEvents);
  monitorThread.detach();

  return env.Undefined();
}
