// Deeangband.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "Deeangband.h"

#include <SDL.h>

#include <iostream>

static bool SDL_system_init();
static bool SDL_event();
static SDL_Window *window;


int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
					   _In_opt_ HINSTANCE hPrevInstance,
					   _In_ LPTSTR    lpCmdLine,
					   _In_ int       nCmdShow)
{
	if(!SDL_system_init()) exit(1);

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

	while (true)
	{
		if(!SDL_event()) break;
	}

	SDL_Quit();
	return 0;
}

static bool SDL_system_init()
{
	if(SDL_Init(SDL_INIT_VIDEO) < 0) return false;
	window = SDL_CreateWindow(GAME_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	if(!window) return false;

	return true;
}

static bool SDL_event()
{
	SDL_Event event;
	SDL_Keycode key;

	while (SDL_PollEvent(&event))
	{
		switch(event.type){

		case SDL_KEYDOWN:
			{
				key=event.key.keysym.sym;
				
				if(key == 27)
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


