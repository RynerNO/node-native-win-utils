#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <napi.h>

Napi::Value Imread(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "String expected for image file path").ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string filename = info[0].ToString().Utf8Value();

    int flags = cv::IMREAD_COLOR;
    if (info.Length() > 1 && info[1].IsNumber())
    {
        flags = info[1].ToNumber().Int32Value();
    }

    cv::Mat image = cv::imread(filename, flags);

    if (image.empty())
    {
        Napi::TypeError::New(env, "Failed to load image").ThrowAsJavaScriptException();
        return env.Null();
    }

    // Create a new Uint8Array with the correct size
    size_t totalBytes = image.total() * image.elemSize();
    Napi::ArrayBuffer arrayBuffer = Napi::ArrayBuffer::New(env, totalBytes);
    Napi::Uint8Array uint8Array = Napi::Uint8Array::New(env, totalBytes, arrayBuffer, 0);

    // Copy the image data to the Uint8Array
    memcpy(uint8Array.Data(), image.data, totalBytes);

    // Create a JavaScript object with 'width', 'height', and 'data' properties
    Napi::Object result = Napi::Object::New(env);
    result.Set("width", image.cols);
    result.Set("height", image.rows);
    result.Set("data", uint8Array);

    return result;
}

Napi::Value Imwrite(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 2 || !info[0].IsString() || !info[1].IsObject())
    {
        Napi::TypeError::New(env, "Invalid arguments. Expected: (string, object)").ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string filename = info[0].ToString().Utf8Value();
    Napi::Object imageData = info[1].As<Napi::Object>();

    if (!imageData.Has("width") || !imageData.Has("height") || !imageData.Has("data"))
    {
        Napi::TypeError::New(env, "Invalid image data object. Expected properties: 'width', 'height', 'data'").ThrowAsJavaScriptException();
        return env.Null();
    }

    int width = imageData.Get("width").ToNumber().Int32Value();
    int height = imageData.Get("height").ToNumber().Int32Value();

    cv::Mat image(height, width, CV_8UC3);

    if (!imageData.Get("data").IsTypedArray())
    {
        Napi::TypeError::New(env, "'data' property must be a TypedArray").ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::TypedArray typedArray = imageData.Get("data").As<Napi::TypedArray>();
    size_t totalBytes = width * height * 3;

    if (typedArray.ByteLength() != totalBytes)
    {
        Napi::TypeError::New(env, "Invalid length of 'data' TypedArray").ThrowAsJavaScriptException();
        return env.Null();
    }

    memcpy(image.data, typedArray.As<Napi::Uint8Array>().Data(), totalBytes);

    bool result = cv::imwrite(filename, image);

    return Napi::Boolean::New(env, result);
}

Napi::Value MatchTemplate(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 2 || !info[0].IsObject() || !info[1].IsObject())
    {
        Napi::TypeError::New(env, "Invalid arguments. Expected: (object, object[, object])").ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::Object srcObj = info[0].As<Napi::Object>();
    Napi::Object templObj = info[1].As<Napi::Object>();
    int method = cv::TM_CCOEFF_NORMED;
    if (info.Length() > 2 && !info[2].IsNumber())
    {
        method = info[2].ToNumber().Int32Value();
    }

    if (!srcObj.Has("data") || !templObj.Has("data"))
    {
        Napi::TypeError::New(env, "Invalid image data objects. Expected properties: 'data'").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!srcObj.Has("width") || !srcObj.Has("height") || !templObj.Has("width") || !templObj.Has("height"))
    {
        Napi::TypeError::New(env, "Invalid image data objects. Expected properties: 'width', 'height'").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!srcObj.Get("data").IsTypedArray() || !templObj.Get("data").IsTypedArray())
    {
        Napi::TypeError::New(env, "TypedArray expected for 'data' properties").ThrowAsJavaScriptException();
        return env.Null();
    }

    int srcWidth = srcObj.Get("width").ToNumber().Int32Value();
    int srcHeight = srcObj.Get("height").ToNumber().Int32Value();
    int templWidth = templObj.Get("width").ToNumber().Int32Value();
    int templHeight = templObj.Get("height").ToNumber().Int32Value();

    Napi::ArrayBuffer srcBufferData = srcObj.Get("data").As<Napi::TypedArray>().ArrayBuffer();
    Napi::ArrayBuffer templBufferData = templObj.Get("data").As<Napi::TypedArray>().ArrayBuffer();

    cv::Mat src(srcHeight, srcWidth, CV_8UC1, srcBufferData.Data());
    cv::Mat templ(templHeight, templWidth, CV_8UC1, templBufferData.Data());

    cv::Mat mask;
    if (info.Length() >= 4 && info[2].IsObject())
    {
        Napi::Object maskObj = info[2].As<Napi::Object>();
        if (maskObj.Has("data") && maskObj.Get("data").IsTypedArray())
        {
            int maskWidth = maskObj.Get("width").ToNumber().Int32Value();
            int maskHeight = maskObj.Get("height").ToNumber().Int32Value();
            Napi::ArrayBuffer maskBufferData = maskObj.Get("data").As<Napi::TypedArray>().ArrayBuffer();
            mask = cv::Mat(maskHeight, maskWidth, CV_8UC1, maskBufferData.Data());
        }
    }

    cv::Mat result;
    if (mask.empty())
    {
        cv::matchTemplate(src, templ, result, cv::TM_CCOEFF_NORMED);
    }
    else
    {
        cv::matchTemplate(src, templ, result, cv::TM_CCOEFF_NORMED, mask);
    }

    double minVal, maxVal;
    cv::Point minLoc, maxLoc;
    cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

    Napi::Object matchResult = Napi::Object::New(env);
    matchResult.Set("minValue", minVal);
    matchResult.Set("maxValue", maxVal);
    Napi::Object minLocation = Napi::Object::New(env);
    minLocation.Set("x", Napi::Number::New(env, minLoc.x));
    minLocation.Set("y", Napi::Number::New(env, minLoc.y));
    matchResult.Set("minLocation", minLocation);

    Napi::Object maxLocation = Napi::Object::New(env);
    maxLocation.Set("x", Napi::Number::New(env, maxLoc.x));
    maxLocation.Set("y", Napi::Number::New(env, maxLoc.y));
    matchResult.Set("maxLocation", maxLocation);

    return matchResult;
}

// Napi::Value ReadImage(const Napi::CallbackInfo &info)
// {
//     Napi::Env env = info.Env();
//     std::string imagePath = info[0].As<Napi::String>();

//     cv::Mat image = cv::imread(imagePath, cv::IMREAD_COLOR);
//     if (image.empty())
//     {
//         Napi::TypeError::New(env, "Could not read the image").ThrowAsJavaScriptException();
//         return env.Null();
//     }

//     cv::Mat grayImage;
//     cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);

//     return Napi::Number::New(env, grayImage.rows * grayImage.cols);
// }