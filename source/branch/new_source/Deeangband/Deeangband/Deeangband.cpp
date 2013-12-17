// Deeangband.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "Deeangband.h"

#include <SDL.h>
#include <SDL_ttf.h>

#include <iostream>
#include <string>

static bool SDL_system_init();
static bool SDL_event();
static SDL_Window *window;


int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
					   _In_opt_ HINSTANCE hPrevInstance,
					   _In_ LPTSTR    lpCmdLine,
					   _In_ int       nCmdShow)
{
	if(!SDL_system_init()) exit(1);
	if(TTF_Init() == -1) exit(1);

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	TTF_Font* font = TTF_OpenFont("ipam.ttf", 18);
	SDL_Surface *surface, *surface2;
	SDL_Color color;
	SDL_Rect src = {0, 0, 300, 200};

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	color.r = 255;
	color.g = 255;
	color.b = 255;
	color.a = 255;

	if(!font) exit(1);
	surface = TTF_RenderUTF8_Blended(font, "D'angband\nAAAAAAAAAAAAAA\nAAAAAAAAA", color);

	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

	while (true)
	{
		SDL_BlitSurface(surface, &src, SDL_GetWindowSurface(window), &src); 
		if(!SDL_event()) break;
		SDL_UpdateWindowSurface(window);
	}

	SDL_Quit();
	return 0;
}

static bool SDL_system_init()
{
	if(SDL_Init(SDL_INIT_VIDEO) < 0) return false;
	window = SDL_CreateWindow(GAME_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
	SDL_GetWindowSurface(window);
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


