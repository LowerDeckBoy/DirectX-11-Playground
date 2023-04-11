#pragma once
#include "../Core/Renderer.hpp"

class Engine
{
public:
	explicit Engine(HINSTANCE hInstance);

	bool Initialize();
	void Run();
	void OnRelease();

private:
	std::unique_ptr<Renderer> m_Renderer{ nullptr };


};

