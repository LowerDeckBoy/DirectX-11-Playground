#include "Engine.hpp"

Engine::Engine(HINSTANCE hInstance)
{
	m_Renderer = std::make_unique<Renderer>(hInstance);
}

bool Engine::Initialize()
{
	m_Renderer->Initialize();

	return true;
}

void Engine::Run()
{
	m_Renderer->Run();
	

}

void Engine::OnRelease()
{
}
