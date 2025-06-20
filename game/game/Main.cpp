// ---------------------------------------------------------------------------
// includes

#include <crtdbg.h> // To check for memory leaks
#include "AEEngine.h"
#include "GameManager.h"
#include "Constants.h"

// ---------------------------------------------------------------------------
// main

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//_CrtSetBreakAlloc(500);

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	AESysInit(hInstance, nCmdShow, kWindowWidth, kWindowHeight, 1, 60, false, NULL);
	GameManager gameManager;
	gameManager.Init();

	gameManager.Update(static_cast<f32>(AEFrameRateControllerGetFrameTime()));

	// free the system
	AESysExit();
}