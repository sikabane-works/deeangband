/*!
 * @file GameSurfaceSDL.cpp
 * @brief ゲームのメインインターフェイス(SDL2.0実装)
 * @date 2014/02/19
 * @author Deskull
 * 2014 Sikabane Works.
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

	initInterfaces();
	return;
}

GameSurfaceSDL::~GameSurfaceSDL(void)
{
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
	return;
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

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
}

void GameSurfaceSDL::Redraw()
{
	Creature dammy;
	SDL_Surface *windowSurface = SDL_GetWindowSurface(window);
	SDL_Rect rect = {0, 0, 0, 0};

	SDL_GetWindowSize(window, &rect.w, &rect.h);

	//SDL_LockSurface(windowSurface);

	SDL_SetRenderDrawColor(renderer, 100, 100, 0, 255);

	SDL_FillRect(windowSurface, &rect, SDL_MapRGBA(windowSurface->format, 50, 20, 10, 255));

	SDL_BlitSurface(surface, &src, windowSurface, &src); 
	SDL_BlitSurface(surface2, &title, windowSurface, &title); 

	//SDL_UnlockSurface(windowSurface);

	ViewCreatureStatus(&dammy);

	//SDL_RenderPresent(renderer);

	//SDL_UpdateWindowSurface

	SDL_UpdateWindowSurface(window);

	return;
}

void GameSurfaceSDL::ViewCreatureStatus(Creature *creaturePtr)
{
	char nameBuf[100], hpBuf[100], mpBuf[100], acBuf[100], evBuf[100], voBuf[100];
	SDL_Surface *windowSurface = SDL_GetWindowSurface(window);
	SDL_Rect rect = {10, 220, 300, 250};
	SDL_Rect namePos = {20, 230, 0 , 0};
	SDL_Rect nameRect = {0, 0, 0 , 0};

	sprintf_s(nameBuf, 100, "名前:%s", creaturePtr->GetName().c_str()); 
	sprintf_s(hpBuf, 100, "HP:%5d/%5d", creaturePtr->GetCurHP(), creaturePtr->GetMaxHP()); 
	sprintf_s(mpBuf, 100, "MP:%5d/%5d", creaturePtr->GetCurMP(), creaturePtr->GetMaxMP()); 
	sprintf_s(acBuf, 100, "AC:%4d", creaturePtr->GetAC()); 
	sprintf_s(evBuf, 100, "EV:%4d", creaturePtr->GetEV()); 
	sprintf_s(voBuf, 100, "VO:%4d", creaturePtr->GetVO()); 

	SDL_Surface *nameSurface, *hpSurface, *mpSurface, *acSurface, *evSurface, *voSurface;
	nameSurface = TTF_RenderUTF8_Blended(font, toUTF8(nameBuf), color);
	hpSurface = TTF_RenderUTF8_Blended(font, toUTF8(hpBuf), color);
	mpSurface = TTF_RenderUTF8_Blended(font, toUTF8(mpBuf), color);
	acSurface = TTF_RenderUTF8_Blended(font, toUTF8(acBuf), color);
	evSurface = TTF_RenderUTF8_Blended(font, toUTF8(evBuf), color);
	voSurface = TTF_RenderUTF8_Blended(font, toUTF8(voBuf), color);

	SDL_FillRect(windowSurface, &rect, SDL_MapRGBA(windowSurface->format, 0, 0, 0, 120));

	nameRect.w = nameSurface->w;
	nameRect.h = nameSurface->h;
	SDL_BlitSurface(nameSurface, &nameRect, windowSurface, &namePos); 

	return;
}

GameCommand GameSurfaceSDL::GetCommand(void)
{
	SDL_Event event;
	SDL_Keycode key;

	while (SDL_PollEvent(&event))
	{
		switch(event.type)
		{

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
