/*!
 *  @file GameSurfaceSDL.cpp
 *  @brief ゲームのメインインターフェイス(SDL2.0実装)
 *  @date 2014/02/19
 *  @author Deskull
 *  2014 Sikabane Works.
 */

#include "stdafx.h"
#include "GameSurfaceSDL.h"

LPSTR toUTF8(LPCSTR str)
{
	const int cchWideChar = ::MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
	LPWSTR lpw = new WCHAR[cchWideChar];
	if(lpw == NULL) return NULL;
	*lpw = L'\0';

	const int nUnicodeCount = ::MultiByteToWideChar(CP_ACP, 0, str, -1, lpw, cchWideChar);
	if(nUnicodeCount <= 0)
	{
		delete[] lpw;
		return NULL;
	}

	const int cchMultiByte = ::WideCharToMultiByte(CP_UTF8, 0, lpw, -1, NULL, 0, NULL, NULL);
	LPSTR lpa = new CHAR[cchMultiByte];
	if(lpa == NULL)
	{
		delete[] lpw;
		return NULL;
	}
	*lpa = '\0';

	const int nMultiCount = ::WideCharToMultiByte(CP_UTF8, 0, lpw, -1, lpa, cchMultiByte, NULL, NULL);
	if(nMultiCount <= 0)
	{
		delete[] lpw;
		delete[] lpa;
		return NULL;
	}
	delete[] lpw;
	return lpa;
}

GameSurfaceSDL::GameSurfaceSDL(void)
{
	if(SDL_Init(SDL_INIT_VIDEO) < 0) return;
	window = SDL_CreateWindow(GAME_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
	SDL_GetWindowSurface(window);
	if(!window) return;

	if(TTF_Init() == -1) return;
	if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) return;
	return;
}

GameSurfaceSDL::~GameSurfaceSDL(void)
{
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
}

void GameSurfaceSDL::initInterfaces(void)
{
	font = TTF_OpenFont("ttf\\ipam.ttf", 18);
	src.x = 0;
	src.y = 0;
	src.w = 300;
	src.h = 200;
	title.x = 0;
	title.y = 0;
	title.w = 512;
	title.h = 512;
	color.r = 255;
	color.g = 223;
	color.b = 200;
	color.a = 255;

	if(!font) exit(1);
	surface = TTF_RenderUTF8_Blended(font, toUTF8("D'angbandテスト"), color);

	rwop = SDL_RWFromFile("img\\Title.png", "rb");
	error = IMG_GetError();
	surface2 = IMG_LoadPNG_RW(rwop);
}

void GameSurfaceSDL::Redraw()
{
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	SDL_BlitSurface(surface, &src, SDL_GetWindowSurface(window), &src); 
	SDL_BlitSurface(surface2, &title, SDL_GetWindowSurface(window), &title); 
	SDL_RenderPresent(renderer);
	SDL_UpdateWindowSurface(window);

	while (true)
	{
		if(!SDL_event()) break;
	}
}

GameCommand GameSurfaceSDL::GetCommand(void)
{
	SDL_Event event;
	SDL_Keycode key;

	while (SDL_PollEvent(&event))
	{
		switch(event.type){

		case SDL_KEYDOWN:
			{
				key=event.key.keysym.sym;

				if(key == SDLK_ESCAPE)
				{
					return GAME_COMMAND_EXIT;
				}
			}
			break;

		case SDL_QUIT:
			return GAME_COMMAND_EXIT;
			break;

		}
	}

	return GAME_COMMAND_REDRAW;
}

bool GameSurfaceSDL::SDL_event(void)
{
	SDL_Event event;
	SDL_Keycode key;

	while (SDL_PollEvent(&event))
	{
		switch(event.type){

		case SDL_KEYDOWN:
			{
				key=event.key.keysym.sym;

				if(key == SDLK_ESCAPE)
				{
					return false;
				}
			}
			break;

		case SDL_QUIT:
			return false;
			break;

		}
	}

	return true;
}
