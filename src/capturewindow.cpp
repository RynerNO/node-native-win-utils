#include <napi.h>
#include <iostream>
#include <Windows.h>
#include <dxgi.h>
#include <inspectable.h>
#include <dxgi1_2.h>
#include <d3d11.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.Graphics.Capture.h>
#include <windows.graphics.capture.interop.h>
#include <windows.graphics.directx.direct3d11.interop.h>
#include <roerrorapi.h>
#include <shlobj_core.h>
#include <dwmapi.h>
#pragma comment(lib,"Dwmapi.lib")
#pragma comment(lib,"windowsapp.lib")


void CaptureWindow(HWND hwndTarget, const std::wstring& outputFilePath)
{
    // Init COM
    winrt::init_apartment(winrt::apartment_type::multi_threaded);
    
    // Create Direct 3D Device
    winrt::com_ptr<ID3D11Device> d3dDevice;

    winrt::check_hresult(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                                           nullptr, 0,D3D11_SDK_VERSION, d3dDevice.put(), nullptr, nullptr));


    winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice device;
    const auto dxgiDevice = d3dDevice.as<IDXGIDevice>();
    {
        winrt::com_ptr<::IInspectable> inspectable;
        winrt::check_hresult(CreateDirect3D11DeviceFromDXGIDevice(dxgiDevice.get(), inspectable.put()));
        device = inspectable.as<winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice>();
    }


    auto idxgiDevice2 = dxgiDevice.as<IDXGIDevice2>();
    winrt::com_ptr<IDXGIAdapter> adapter;
    winrt::check_hresult(idxgiDevice2->GetParent(winrt::guid_of<IDXGIAdapter>(), adapter.put_void()));
    winrt::com_ptr<IDXGIFactory2> factory;
    winrt::check_hresult(adapter->GetParent(winrt::guid_of<IDXGIFactory2>(), factory.put_void()));

    ID3D11DeviceContext* d3dContext = nullptr;
    d3dDevice->GetImmediateContext(&d3dContext);

    RECT rect{};
    DwmGetWindowAttribute(hwndTarget, DWMWA_EXTENDED_FRAME_BOUNDS, &rect, sizeof(RECT));
    const auto size = winrt::Windows::Graphics::SizeInt32{rect.right - rect.left, rect.bottom - rect.top};

    winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool m_framePool =
        winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool::Create(
            device,
            winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
            2,
            size);

    const auto activationFactory = winrt::get_activation_factory<
        winrt::Windows::Graphics::Capture::GraphicsCaptureItem>();
    auto interopFactory = activationFactory.as<IGraphicsCaptureItemInterop>();
    winrt::Windows::Graphics::Capture::GraphicsCaptureItem captureItem = {nullptr};
    interopFactory->CreateForWindow(hwndTarget, winrt::guid_of<ABI::Windows::Graphics::Capture::IGraphicsCaptureItem>(),
                                    reinterpret_cast<void**>(winrt::put_abi(captureItem)));

    auto isFrameArrived = false;
    winrt::com_ptr<ID3D11Texture2D> texture;
    const auto session = m_framePool.CreateCaptureSession(captureItem);
    m_framePool.FrameArrived([&](auto& framePool, auto&)
    {
        if (isFrameArrived) return;
        auto frame = framePool.TryGetNextFrame();

        struct __declspec(uuid("A9B3D012-3DF2-4EE3-B8D1-8695F457D3C1"))
            IDirect3DDxgiInterfaceAccess : ::IUnknown
        {
            virtual HRESULT __stdcall GetInterface(GUID const& id, void** object) = 0;
        };

        auto access = frame.Surface().as<IDirect3DDxgiInterfaceAccess>();
        access->GetInterface(winrt::guid_of<ID3D11Texture2D>(), texture.put_void());
        isFrameArrived = true;
        return;
    });


    session.IsCursorCaptureEnabled(false);
    session.StartCapture();


    // Message pump
    MSG msg;
    clock_t timer = clock();
    while (!isFrameArrived)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
            DispatchMessage(&msg);

        if (clock() - timer > 20000)
        {
            // TODO: try to make here a better error handling
            return;
        }
    }

    session.Close();

    D3D11_TEXTURE2D_DESC capturedTextureDesc;
    texture->GetDesc(&capturedTextureDesc);

    capturedTextureDesc.Usage = D3D11_USAGE_STAGING;
    capturedTextureDesc.BindFlags = 0;
    capturedTextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    capturedTextureDesc.MiscFlags = 0;

    winrt::com_ptr<ID3D11Texture2D> userTexture = nullptr;
    winrt::check_hresult(d3dDevice->CreateTexture2D(&capturedTextureDesc, NULL, userTexture.put()));

    d3dContext->CopyResource(userTexture.get(), texture.get());


    D3D11_MAPPED_SUBRESOURCE resource;
    winrt::check_hresult(d3dContext->Map(userTexture.get(), NULL, D3D11_MAP_READ, 0, &resource));

    BITMAPINFO lBmpInfo;

    // BMP 32 bpp
    ZeroMemory(&lBmpInfo, sizeof(BITMAPINFO));
    lBmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    lBmpInfo.bmiHeader.biBitCount = 32;
    lBmpInfo.bmiHeader.biCompression = BI_RGB;
    lBmpInfo.bmiHeader.biWidth = capturedTextureDesc.Width;
    lBmpInfo.bmiHeader.biHeight = capturedTextureDesc.Height;
    lBmpInfo.bmiHeader.biPlanes = 1;
    lBmpInfo.bmiHeader.biSizeImage = capturedTextureDesc.Width * capturedTextureDesc.Height * 4;
    
    std::unique_ptr<BYTE> pBuf(new BYTE[lBmpInfo.bmiHeader.biSizeImage]);
    UINT lBmpRowPitch = capturedTextureDesc.Width * 4;
    auto sptr = static_cast<BYTE*>(resource.pData);
    auto dptr = pBuf.get() + lBmpInfo.bmiHeader.biSizeImage - lBmpRowPitch;

    UINT lRowPitch = std::min<UINT>(lBmpRowPitch, resource.RowPitch);

    for (size_t h = 0; h < capturedTextureDesc.Height; ++h)
    {
        memcpy_s(dptr, lBmpRowPitch, sptr, lRowPitch);
        sptr += resource.RowPitch;
        dptr -= lBmpRowPitch;
    }

       // Save bitmap buffer into the provided output file path
    FILE* lfile = nullptr;

    if (auto lerr = _wfopen_s(&lfile, outputFilePath.c_str(), L"wb"); lerr != 0)
        return;

    if (lfile != nullptr)
    {
        BITMAPFILEHEADER bmpFileHeader;

        bmpFileHeader.bfReserved1 = 0;
        bmpFileHeader.bfReserved2 = 0;
        bmpFileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + lBmpInfo.bmiHeader.biSizeImage;
        bmpFileHeader.bfType = 'MB';
        bmpFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

        fwrite(&bmpFileHeader, sizeof(BITMAPFILEHEADER), 1, lfile);
        fwrite(&lBmpInfo.bmiHeader, sizeof(BITMAPINFOHEADER), 1, lfile);
        fwrite(pBuf.get(), lBmpInfo.bmiHeader.biSizeImage, 1, lfile);

        fclose(lfile);
    }

}