#include <napi.h>

// Global variable to store the JavaScript callback function
Napi::FunctionReference globalJsCallback;

// KeyPressEventHandler function
void KeyPressEventHandler(Napi::Env env, int keyCode) {
  Napi::HandleScope scope(env);

  // Create arguments for calling the callback function in Node.js
  Napi::Value argv[] = {
    Napi::Number::New(env, keyCode)
  };

  // Call the callback function in Node.js
  globalJsCallback.Call({argv[0]});
}

// Function called from JavaScript to set the callback function
Napi::Value SetKeyPressCallback(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  
  // Get the callback function from the arguments
  Napi::Function jsCallback = info[0].As<Napi::Function>();
  
  // Store the callback function in the global variable
  globalJsCallback.Reset(jsCallback, 1);
  
  return env.Undefined();
}