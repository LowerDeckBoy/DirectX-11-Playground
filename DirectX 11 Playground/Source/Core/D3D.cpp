#include "D3D.hpp"

D3D::D3D(HINSTANCE hInstance) : Window(hInstance)
{
    //m_Camera = new Camera();
    m_MainFont = nullptr;
}

D3D::~D3D()
{
    delete m_MainFont;
    DX_Mouse = NULL;
    DX_Keyboard = NULL;
    //delete m_Camera;

    ReleaseCOM(m_SSPointWrap);
    ReleaseCOM(m_SSPointClamp);
    ReleaseCOM(m_SSLinearWrap);
    ReleaseCOM(m_SSLinearClamp);
    ReleaseCOM(m_SSAnisotropyWrap);
    ReleaseCOM(m_SSAnisotropyClamp);
    ReleaseCOM(m_SSAnisotropyMin);
    ReleaseCOM(m_SSAnisotropyMax);
    ReleaseCOM(m_BlendAlpha);
    ReleaseCOM(m_BlendOpaque);

    ReleaseCOM(m_RasterizerWireframe);
    ReleaseCOM(m_RasterizerSolid);
    ReleaseCOM(m_RenderTargetView);
    ReleaseCOM(m_DepthStencilBuffer);
    ReleaseCOM(m_DepthStencilView);
    ReleaseCOM(m_SwapChain)

    if (m_Context)
    {
        m_Context.Get()->ClearState();
        m_Context.Get()->Flush();
    }

    ReleaseCOM(m_Context);
    ReleaseCOM(m_Device);

    //OutputDebugStringA("D3D destructor called.\n"); -> Debug Only
}

bool D3D::Initialize()
{
	if (!InitializeD3D())
	{
		OutputDebugStringA("\nFailed to Initialize D3D!\n");
		return false;
	}

	InitializeStates();

   // IMGUI_CHECKVERSION();
	ImGui::CreateContext();

    ImGuiIO& IO = ImGui::GetIO();
    //IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    //IO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGuiStyle& Style = ImGui::GetStyle();
    Style.WindowRounding = 5.0f;
    Style.WindowBorderSize = 0.0f;
    
	ImGui_ImplWin32_Init(GetHWND());
	ImGui_ImplDX11_Init(GetDevice(), GetContext());

    const float fontSize = 15.0f;
    m_MainFont = IO.Fonts->AddFontFromFileTTF("Assets/Fonts/CascadiaCode-Bold.ttf", fontSize);

    m_Camera->Setup(GetDisplayResolution().AspectRatio);

	return true;
}

bool D3D::InitializeD3D()
{
    if (!Window::Initialize())
        return false;

    uint32_t createDeviceFlags = 0;
#if defined (_DEBUG) || (DEBUG)
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;

    ThrowIfFailed(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, nullptr, 0, D3D11_SDK_VERSION, device.GetAddressOf(), &featureLevel, context.GetAddressOf()));

    // Casting to higher versions
    ThrowIfFailed(device.As(&m_Device));
    ThrowIfFailed(context.As(&m_Context));

    DXGI_SWAP_CHAIN_DESC1 scDesc{};
    scDesc.BufferCount = 2;
    scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scDesc.SampleDesc.Count = 1;
    scDesc.SampleDesc.Quality = 0;
    scDesc.Width = static_cast<uint32_t>(GetDisplayResolution().Width);
    scDesc.Height = static_cast<uint32_t>(GetDisplayResolution().Height);
    scDesc.Scaling = DXGI_SCALING_NONE;
    scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc{};
    fullscreenDesc.RefreshRate.Numerator = 0;
    fullscreenDesc.RefreshRate.Denominator = 0;
    fullscreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    fullscreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    fullscreenDesc.Windowed = TRUE;

    ComPtr<IDXGIDevice> dxgiDevice;
    ThrowIfFailed(m_Device.As(&dxgiDevice));

    ComPtr<IDXGIAdapter> dxgiAdapter;
    ThrowIfFailed(dxgiDevice->GetAdapter(&dxgiAdapter));

    ComPtr<IDXGIFactory2> dxgiFactory;
    // For Init then Casting
    ComPtr<IDXGISwapChain1> swapchain;
    ThrowIfFailed(dxgiAdapter->GetParent(__uuidof(IDXGIFactory4), &dxgiFactory));
    ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(m_Device.Get(), GetHWND(), &scDesc, &fullscreenDesc, nullptr, swapchain.GetAddressOf()));
    ThrowIfFailed(swapchain.As(&m_SwapChain));

    ThrowIfFailed(::DirectInput8Create(GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&DX_Input, NULL));
    ThrowIfFailed(DX_Input->CreateDevice(GUID_SysKeyboard, &DX_Keyboard, NULL));
    ThrowIfFailed(DX_Keyboard->SetDataFormat(&c_dfDIKeyboard));
    ThrowIfFailed(DX_Keyboard->SetCooperativeLevel(GetHWND(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
    ThrowIfFailed(DX_Input->CreateDevice(GUID_SysMouse, &DX_Mouse, NULL));
    ThrowIfFailed(DX_Mouse->SetDataFormat(&c_dfDIMouse));
    ThrowIfFailed(DX_Mouse->SetCooperativeLevel(GetHWND(), DISCL_NONEXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND));


#if defined (_DEBUG) || (DEBUG)
    //https://learn.microsoft.com/en-us/windows/win32/api/d3d11sdklayers/nn-d3d11sdklayers-id3d11infoqueue
    ID3D11Debug* debugDevice = nullptr;
    ThrowIfFailed(GetDevice()->QueryInterface(__uuidof(ID3D11Debug), (void**)(&debugDevice)));
    if (debugDevice)
    {
        //| D3D11_RLDO_DETAIL
        debugDevice->ReportLiveDeviceObjects(D3D11_RLDO_IGNORE_INTERNAL );
        //debugDevice->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
        debugDevice->Release();
    }

#endif  

    OnResize();

    return true;
}

void D3D::OnResize()
{
    GetContext()->OMSetRenderTargets(0, 0, 0);

    if (GetRenderTargetView())
        GetRenderTargetView()->Release();
    if (GetDepthStencilView())
        GetDepthStencilView()->Release();
    if (GetDepthStencilBuffer())
        GetDepthStencilBuffer()->Release();

    if (GetContext())
    {
        GetContext()->ClearState();
        GetContext()->Flush();
    }
    GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    ThrowIfFailed(GetSwapChain()->ResizeBuffers(0, 0, 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0));

    ID3D11Texture2D* pBackBuffer = nullptr;
    ThrowIfFailed(GetSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer));
    ThrowIfFailed(GetDevice()->CreateRenderTargetView(pBackBuffer, nullptr, m_RenderTargetView.GetAddressOf()));
    pBackBuffer->Release();

    GetContext()->OMSetRenderTargets(1, GetRenderTargetViewAddress(), nullptr);

    D3D11_TEXTURE2D_DESC depthDesc{};
    depthDesc.Width     = static_cast<uint32_t>(GetDisplayResolution().Width);
    depthDesc.Height    = static_cast<uint32_t>(GetDisplayResolution().Height);
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;

    depthDesc.SampleDesc.Count = 1;
    depthDesc.SampleDesc.Quality = 0;

    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthDesc.CPUAccessFlags = 0;
    depthDesc.MiscFlags = 0;

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format = depthDesc.Format;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;

    ThrowIfFailed(GetDevice()->CreateTexture2D(&depthDesc, nullptr, m_DepthStencilBuffer.GetAddressOf()));
    ThrowIfFailed(GetDevice()->CreateDepthStencilView(m_DepthStencilBuffer.Get(), &dsvDesc, m_DepthStencilView.GetAddressOf()));

    
    SetBackbuffer();
    m_Camera->OnAspectRatioChange(GetDisplayResolution().AspectRatio);
}

void D3D::InitializeStates()
{
    // Rasterizers
    {
        D3D11_RASTERIZER_DESC rasterizer = CommonStates::RS_Solid();
        ThrowIfFailed(GetDevice()->CreateRasterizerState(&rasterizer, m_RasterizerSolid.GetAddressOf()));

        rasterizer = CommonStates::RS_Wireframe();
        ThrowIfFailed(GetDevice()->CreateRasterizerState(&rasterizer, m_RasterizerWireframe.GetAddressOf()));

        // Samplers
        D3D11_SAMPLER_DESC sampler = CommonStates::PointWrap();
        ThrowIfFailed(m_Device->CreateSamplerState(&sampler, m_SSPointWrap.GetAddressOf()));

        sampler = CommonStates::PointClamp();
        ThrowIfFailed(m_Device->CreateSamplerState(&sampler, m_SSPointClamp.GetAddressOf()));

        sampler = CommonStates::LinearWrap();
        ThrowIfFailed(m_Device->CreateSamplerState(&sampler, m_SSLinearWrap.GetAddressOf()));

        sampler = CommonStates::LinearClamp();
        ThrowIfFailed(m_Device->CreateSamplerState(&sampler, m_SSLinearClamp.GetAddressOf()));

        sampler = CommonStates::AnisotropyWrap();
        ThrowIfFailed(m_Device->CreateSamplerState(&sampler, m_SSAnisotropyWrap.GetAddressOf()));

        sampler = CommonStates::AnisotropyClamp();
        ThrowIfFailed(m_Device->CreateSamplerState(&sampler, m_SSAnisotropyClamp.GetAddressOf()));

        sampler = CommonStates::AnisotropyMin();
        ThrowIfFailed(m_Device->CreateSamplerState(&sampler, m_SSAnisotropyMin.GetAddressOf()));

        sampler = CommonStates::AnisotropyMax();
        ThrowIfFailed(m_Device->CreateSamplerState(&sampler, m_SSAnisotropyMax.GetAddressOf()));
    }

    // Blend states
    {
        D3D11_BLEND_DESC blend = CommonStates::BlendAlpha();
        ThrowIfFailed(m_Device.Get()->CreateBlendState(&blend, m_BlendAlpha.GetAddressOf()));

        blend = CommonStates::BlendOpaque();
        ThrowIfFailed(m_Device.Get()->CreateBlendState(&blend, m_BlendOpaque.GetAddressOf()));
    }

    // Depth Stencil States
    {
        D3D11_DEPTH_STENCIL_DESC depth = CommonStates::DepthDefault();
        ThrowIfFailed(m_Device.Get()->CreateDepthStencilState(&depth, m_DSSDefault.GetAddressOf()));

        depth = CommonStates::DepthNone();
        ThrowIfFailed(m_Device.Get()->CreateDepthStencilState(&depth, m_DSSNone.GetAddressOf()));
    }

}

void D3D::SetBackbuffer()
{
    D3D11_VIEWPORT viewport{};
    viewport.Width      = static_cast<float>(GetDisplayResolution().Width);
    viewport.Height     = static_cast<float>(GetDisplayResolution().Height);
    viewport.MinDepth   = 0.0f;
    viewport.MaxDepth   = 1.0f;
    viewport.TopLeftX   = 0;
    viewport.TopLeftY   = 0;
    m_DisplayResolution.AspectRatio = static_cast<float>((float)GetDisplayResolution().Width / (float)GetDisplayResolution().Height);
    GetContext()->RSSetViewports(1, &viewport);
    GetContext()->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), GetDepthStencilView());
}

void D3D::ClearBackbuffer(const float background[4])
{
    GetContext()->ClearRenderTargetView(GetRenderTargetView(), background);
    GetContext()->ClearDepthStencilView(GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

ID3D11RasterizerState* D3D::SetRasterizerState() const
{
    return (bSolidState) ? m_RasterizerSolid.Get() : m_RasterizerWireframe.Get();
}

ID3D11SamplerState* const* D3D::SetMainSamplerState(uint32_t Select) const
{
    switch (Select)
    {
    case 0:
        return m_SSPointWrap.GetAddressOf();
        
    case 1:
        return m_SSPointClamp.GetAddressOf();

    case 2:
        return m_SSLinearWrap.GetAddressOf();

    case 3:
        return m_SSLinearClamp.GetAddressOf();

    case 4:
        return m_SSAnisotropyWrap.GetAddressOf();

    case 5:
        return m_SSAnisotropyClamp.GetAddressOf();

    case 6:
        return m_SSAnisotropyMin.GetAddressOf();

    case 7:
        return m_SSAnisotropyMax.GetAddressOf();

    default:
        return m_SSAnisotropyWrap.GetAddressOf();

    }

    return m_SSAnisotropyWrap.GetAddressOf();
}

ID3D11SamplerState* const* D3D::SetSecondSamplerState(uint32_t Select) const
{
    switch (Select)
    {
    case 0:
        return m_SSPointWrap.GetAddressOf();

    case 1:
        return m_SSPointClamp.GetAddressOf();

    case 2:
        return m_SSLinearWrap.GetAddressOf();

    case 3:
        return m_SSLinearClamp.GetAddressOf();

    case 4:
        return m_SSAnisotropyWrap.GetAddressOf();

    case 5:
        return m_SSAnisotropyClamp.GetAddressOf();

    case 6:
        return m_SSAnisotropyMin.GetAddressOf();

    case 7:
        return m_SSAnisotropyMax.GetAddressOf();

    default:
        return m_SSAnisotropyWrap.GetAddressOf();

    }

    return m_SSAnisotropyWrap.GetAddressOf();
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, uint32_t msg, WPARAM wParam, LPARAM lParam);
LRESULT D3D::WindowProc(HWND hWnd, uint32_t msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_ACTIVATE:
    {
        if (LOWORD(wParam) == WA_INACTIVE)
        {
            bAppPaused = true;
            GetTimer()->Stop();
        }
        else
        {
            bAppPaused = false;
            GetTimer()->Start();
        }

        return 0;
    }

    case WM_SIZE:
    {
        m_DisplayResolution.Width = LOWORD(lParam);
        m_DisplayResolution.Height = HIWORD(lParam);

        if (GetDevice())
        {
            if (wParam == SIZE_MINIMIZED)
            {
                bMinimized = true;
                bMaximized = false;
                bAppPaused = true;
            }
            else if (wParam == SIZE_MAXIMIZED)
            {
                bMinimized = false;
                bMaximized = true;
                bAppPaused = false;
                OnResize();
            }
            else if (wParam == SIZE_RESTORED)
            {
                if (bMinimized)
                {
                    bAppPaused = false;
                    bMinimized = false;
                    OnResize();
                }
                else if (bMaximized)
                {
                    bAppPaused = false;
                    bMaximized = false;
                    OnResize();
                }
                else if (bIsResizing)
                {
                    //OnResize();
                }
                else
                {
                    OnResize();
                }
            }
        }
        return 0;
    }

    case WM_ENTERSIZEMOVE:
    {
        bAppPaused = true;
        bIsResizing = true;
        GetTimer()->Stop();

        return 0;
    }

    case WM_EXITSIZEMOVE:
    {
        bAppPaused = false;
        bIsResizing = false;
        GetTimer()->Start();
        OnResize();

        return 0;
    }

    case WM_CLOSE:
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    }
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}
