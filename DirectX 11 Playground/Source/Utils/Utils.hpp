#pragma once
#include <exception>
#include <string>
#include <filesystem>
#include <cstdint>
#include <cassert>

#define SafeRelease(x)    { if (x) { x->Release(); x = nullptr; } }
#define ReleaseCOM(ppT)   { if (ppT) { ppT.Reset(); } }

inline void ThrowIfFailed(HRESULT hResult)
{
	if (FAILED(hResult))
		throw std::exception();
}

inline void ThrowIfFailed(HRESULT hResult, const std::string_view& Message)
{
	if (FAILED(hResult))
	{
		::OutputDebugStringA(Message.data());
		throw std::exception();
	}
}
