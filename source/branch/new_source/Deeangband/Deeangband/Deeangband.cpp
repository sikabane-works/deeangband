// Deeangband.cpp : �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include "Deeangband.h"
#include "GameSurfaceSDL.h"

#include <iostream>
#include <string>

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
					   _In_opt_ HINSTANCE hPrevInstance,
					   _In_ LPTSTR    lpCmdLine,
					   _In_ int       nCmdShow)
{
	GameSurfaceSDL gSurface;
	gSurface.test();
	return 0;
}




