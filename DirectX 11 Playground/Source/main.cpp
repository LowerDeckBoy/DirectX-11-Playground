#include "Engine/Engine.hpp"

// https://github.com/GPUOpen-LibrariesAndSDKs/GPUParticles11
// https://github.com/simco50/D3D12_Research

_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
	Engine app(hInstance);

	if (!app.Initialize())
		return -1;

	app.Run();

	return 0;
}

