#include "Window.hpp"

#include <dwmapi.h>
#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

namespace { Window* window = 0; }

LRESULT CALLBACK MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return window->WindowProc(hWnd, msg, wParam, lParam);
}

DisplayResolution Window::m_DisplayResolution{ 1280.0f, 800.0f, 1280.0f / 800.0f };

Window::Window(HINSTANCE hInstance) :
	m_hWnd(nullptr),
	m_hInstance(hInstance)
{
	window = this;
	m_Timer = new Timer();
}

Window::~Window()
{
	delete m_Timer;
	::UnregisterClass(m_ClassName, m_hInstance);

	if (m_hInstance)
		m_hInstance = nullptr;
	if (m_hWnd)
		m_hWnd = nullptr;

	if (window)
		window = nullptr;

}

bool Window::Initialize()
{
	WNDCLASSEX wcex{};
	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= MsgProc;
	wcex.lpszClassName	= m_ClassName;
	wcex.hInstance		= m_hInstance;
	wcex.hbrBackground	= (HBRUSH)(::CreateSolidBrush(RGB(20, 20, 20)));

	if (!::RegisterClassEx(&wcex))
	{
		::OutputDebugStringA("\nFailed to Register Window Class!\n");
		return false;
	}

	m_WindowRect = { 0, 0, static_cast<LONG>(GetDisplayResolution().Width), static_cast<LONG>(GetDisplayResolution().Height) };
	::AdjustWindowRect(&m_WindowRect, WS_OVERLAPPEDWINDOW, false);
	int width  = m_WindowRect.right  - m_WindowRect.left;
	int height = m_WindowRect.bottom - m_WindowRect.top;

	m_hWnd = ::CreateWindow(GetClassName(), GetWindowName(),
							WS_OVERLAPPEDWINDOW, 
							CW_USEDEFAULT, CW_USEDEFAULT, 
							width, height, 0, 0, 
							GetHInstance(), 0);

	if (!m_hWnd)
	{
		::MessageBoxA(m_hWnd, "Failed to create window!", "Error", MB_OK);
		return false;
	}

	// Center window position on startup
	int32_t xPos = (::GetSystemMetrics(SM_CXSCREEN) - m_WindowRect.right) / 2;
	int32_t yPos = (::GetSystemMetrics(SM_CYSCREEN) - m_WindowRect.bottom) / 2;
	::SetWindowPos(GetHWND(), 0, xPos, yPos, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

	BOOL bDarkMode = TRUE;
	::DwmSetWindowAttribute(m_hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &bDarkMode, sizeof(bDarkMode));

	return true;
}

int32_t Window::Run()
{
	Show();
	MSG msg{};

	GetTimer()->Reset();

	while (msg.message != WM_QUIT)
	{
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else
		{
			GetTimer()->Tick();
			GetTimer()->GetFrameStats();

			if (!bAppPaused)
			{
				CameraInputs(GetTimer()->DeltaTime());
				Render();
				Update(GetTimer()->DeltaTime());
			}
			else
			{
				::Sleep(100);
			}
		}
	}

	return static_cast<int32_t>(msg.wParam);
}

void Window::Show()
{
	::SendMessage(GetHWND(), WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	::SetForegroundWindow(GetHWND());
	::ShowWindow(GetHWND(), SW_SHOW);
	::UpdateWindow(GetHWND());
}

void Window::HideCursor()
{
	while (::ShowCursor(FALSE) >= 0)
		bIsCursorVisible = false;
}

void Window::ShowCursor()
{
	while (::ShowCursor(TRUE) < 0) 
		bIsCursorVisible = true;
}
