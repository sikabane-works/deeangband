/*!
 * @file GameSurfaceSDL.cpp
 * @brief ゲームのメインインターフェイス(SDL2.0実装)
 * @date 2014/02/19
 * @author Deskull
 * 2014 Sikabane Works.
 */

#include <vector>
#include <string>
#include "stdafx.h"
#include "GameSurfaceSDL.h"

std::string toUTF8(LPCSTR str)
{
	const int cchWideChar = ::MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
	std::vector<WCHAR> lpw(cchWideChar);

	const int nUnicodeCount = ::MultiByteToWideChar(CP_ACP, 0, str, -1, &lpw[0], cchWideChar);
	if(nUnicodeCount <= 0)
	{
		return "";
	}

	const int cchMultiByte = ::WideCharToMultiByte(CP_UTF8, 0, &lpw[0], -1, NULL, 0, NULL, NULL);
	std::vector<CHAR> lpa(cchMultiByte);

	const int nMultiCount = ::WideCharToMultiByte(CP_UTF8, 0, &lpw[0], -1, &lpa[0], cchMultiByte, NULL, NULL);
	if(nMultiCount <= 0)
	{
		return "";
	}
	return std::string(&lpa[0]);
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

	viewCreaturePtr = NULL;

	return;
}

GameSurfaceSDL::~GameSurfaceSDL(void)
{
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
	return;
}

void GameSurfaceSDL::pushAnyKey(void)
{
	SDL_Event event;
	SDL_Keycode key;

	while(true)
	{
		while (SDL_PollEvent(&event))
		{
			key=event.key.keysym.sym;
			if(event.type == SDL_KEYDOWN) return;
		}
	}
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
	surface = TTF_RenderUTF8_Blended(font, toUTF8("D'angbandテスト").c_str(), color);

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

	SDL_SetRenderDrawColor(renderer, 100, 100, 0, 255);

	SDL_FillRect(windowSurface, &rect, SDL_MapRGBA(windowSurface->format, 50, 20, 10, 255));

	SDL_BlitSurface(surface, &src, windowSurface, &src); 
	SDL_BlitSurface(surface2, &title, windowSurface, &title); 

	if(viewCreaturePtr) drawCreatureStatus(viewCreaturePtr);

	SDL_UpdateWindowSurface(window);

	return;
}

void GameSurfaceSDL::ViewCreatureStatus(Creature *creaturePtr)
{
	viewCreaturePtr = creaturePtr;
	Redraw();
	pushAnyKey();
	viewCreaturePtr = NULL;
}

void GameSurfaceSDL::drawCreatureStatus(Creature *creaturePtr)
{
	int id;
	SDL_Surface *windowSurface = SDL_GetWindowSurface(window);

	enum CREATURE_STATUS_VIEW_POSITION
	{
		POS_NAME,
		POS_HP,
		POS_MP,
		POS_AC,
		POS_EV,
		POS_VO,
		POS_STR,
		POS_INT,
		POS_WIS,
		POS_DEX,
		POS_CON,
		POS_CHA,
		POS_SOUL,
		POS_MAX
	};

	SDL_Rect CreatureStatusViewPosition[POS_MAX] =
	{
		{10, 10, 0, 0},
		{10, 30, 0, 0},
		{10, 50, 0, 0},
		{10, 70, 0, 0},
		{10, 90, 0, 0},
		{10, 110, 0, 0},
		{200, 30, 0, 0},
		{200, 50, 0, 0},
		{200, 70, 0, 0},
		{200, 90, 0, 0},
		{200, 110, 0, 0},
		{200, 130, 0, 0},
		{200, 150, 0, 0},
	};

	SDL_Rect CreatureStatusViewRect[POS_MAX];

	const int STATUS_BUFSIZE = 100;
	char statusBuf[POS_MAX][STATUS_BUFSIZE];
	SDL_Surface *statusSurface[POS_MAX];

	SDL_Rect masterRect = {10, 10, 490, 450};

	sprintf_s(statusBuf[POS_NAME], STATUS_BUFSIZE, "名前:%s", creaturePtr->GetName().c_str()); 
	sprintf_s(statusBuf[POS_HP], STATUS_BUFSIZE, "HP:%5d/%5d", creaturePtr->GetCurHP(), creaturePtr->GetMaxHP()); 
	sprintf_s(statusBuf[POS_MP], STATUS_BUFSIZE, "MP:%5d/%5d", creaturePtr->GetCurMP(), creaturePtr->GetMaxMP()); 
	sprintf_s(statusBuf[POS_AC], STATUS_BUFSIZE, "AC:%4d", creaturePtr->GetAC()); 
	sprintf_s(statusBuf[POS_EV], STATUS_BUFSIZE, "EV:%4d", creaturePtr->GetEV()); 
	sprintf_s(statusBuf[POS_VO], STATUS_BUFSIZE, "VO:%4d", creaturePtr->GetVO()); 
	sprintf_s(statusBuf[POS_STR], STATUS_BUFSIZE, "腕力:%4d", creaturePtr->GetCurrentStatus(CS_STR)); 
	sprintf_s(statusBuf[POS_INT], STATUS_BUFSIZE, "知力:%4d", creaturePtr->GetCurrentStatus(CS_INT)); 
	sprintf_s(statusBuf[POS_WIS], STATUS_BUFSIZE, "賢さ:%4d", creaturePtr->GetCurrentStatus(CS_WIS)); 
	sprintf_s(statusBuf[POS_DEX], STATUS_BUFSIZE, "器用:%4d", creaturePtr->GetCurrentStatus(CS_DEX)); 
	sprintf_s(statusBuf[POS_CON], STATUS_BUFSIZE, "耐久:%4d", creaturePtr->GetCurrentStatus(CS_CON)); 
	sprintf_s(statusBuf[POS_CHA], STATUS_BUFSIZE, "魅力:%4d", creaturePtr->GetCurrentStatus(CS_CHA)); 
	sprintf_s(statusBuf[POS_SOUL], STATUS_BUFSIZE, "ソウル:%4d", creaturePtr->GetCurrentSoul()); 

	for(id = 0; id < POS_MAX; id++)
	{
		statusSurface[id] = TTF_RenderUTF8_Blended(font, toUTF8(statusBuf[id]).c_str(), color);
		CreatureStatusViewRect[id].x = 0;
		CreatureStatusViewRect[id].y = 0;
		CreatureStatusViewRect[id].w = statusSurface[id]->w;
		CreatureStatusViewRect[id].h = statusSurface[id]->h;
		CreatureStatusViewPosition[id].x += masterRect.x;
		CreatureStatusViewPosition[id].y += masterRect.y;
	}

	SDL_FillRect(windowSurface, &masterRect, SDL_MapRGBA(windowSurface->format, 0, 0, 0, 120));

	for(id = 0; id < POS_MAX; id++)
	{
		SDL_BlitSurface(statusSurface[id], &CreatureStatusViewRect[id], windowSurface, &CreatureStatusViewPosition[id]); 
	}

	for(id = 0; id < POS_MAX; id++)
	{
		SDL_FreeSurface(statusSurface[id]);
	}
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

				switch(key)
				{
				case SDLK_ESCAPE:
					return GAME_COMMAND_EXIT;
				case SDLK_c:
					return GAME_COMMAND_VIEW_PLAYER_STATUS;
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
