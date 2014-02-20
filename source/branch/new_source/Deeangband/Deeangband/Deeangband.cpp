// Deeangband.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "Deeangband.h"
#include "GameWorld.h"
#include "GameSurfaceSDL.h"

#include <iostream>
#include <string>

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
					   _In_opt_ HINSTANCE hPrevInstance,
					   _In_ LPTSTR    lpCmdLine,
					   _In_ int       nCmdShow)
{
	GameSurface gSurf;
	GameSurfaceSDL gSurface;
	GameWorld gWorld(&gSurf);
	gSurface.test();
	return 0;
}




