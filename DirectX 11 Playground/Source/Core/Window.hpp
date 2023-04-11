#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <iostream>
#include "../Utils/Utils.hpp"
#include "../Utils/Timer.hpp"

struct DisplayResolution
{
	float Width;
	float Height;
	float AspectRatio;
};

class Window
{
public:
	Window(HINSTANCE hInstance);
	Window(const Window&) = delete;
	virtual ~Window();

	bool Initialize();
	int32_t Run();

	virtual LRESULT WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) = 0;

	virtual void OnResize() = 0;
	virtual void CameraInputs(float DeltaTime) = 0;
	virtual void Update(float DeltaTime) = 0;
	virtual void Render() = 0;

	void Show();
public:
	static DisplayResolution GetDisplayResolution() { return m_DisplayResolution; }

	inline HWND GetHWND() const { return m_hWnd; }
	inline HINSTANCE GetHInstance() const { return m_hInstance; }
	inline LPCWSTR GetWindowName() const { return m_WindowName; }
	inline LPCWSTR GetClassName() const { return m_ClassName; }

	Timer* GetTimer() const { return m_Timer; }

	static DisplayResolution m_DisplayResolution;

	void HideCursor();
	void ShowCursor();


private:
	HWND m_hWnd;
	HINSTANCE m_hInstance;

	// Window naming
	LPCWSTR m_WindowName{ L"Main Window" };
	LPCWSTR m_ClassName	{ L"Main Class" };

	RECT m_WindowRect{ 1280, 800 };

public:
	bool bAppPaused { false };
	bool bMinimized { false };
	bool bMaximized { false };
	bool bIsResizing{ false };

	bool bIsCursorVisible{ false };

protected:
	Timer* m_Timer;


};
