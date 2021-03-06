/*!
 * @file GameSurfaceSDL.h
 * @brief GameSurfaceSDLクラスとその他付随要素のプロトタイプ
 * @date 2014/02/19
 * @author Deskull
 * 2014 Sikabane Works.
 */

#pragma once

#include "stdafx.h"

#include "Deeangband.h"
#include "GameSurface.h"
#include <SDL.h>
#include <SDL_image.h>
//#include <SDL_gfx.h>
#include <SDL_ttf.h>

/*! 
 * @class GameSurfaceSDL
 * @brief ゲームのメインインターフェイスクラス(SDL実装)
 */
class GameSurfaceSDL : public GameSurface
{
private:
	Creature *viewCreaturePtr;

	SDL_Renderer *renderer; 
	SDL_RWops *rwop;
	string error;
	SDL_Window *window;
	TTF_Font* font;
	SDL_Surface *surface, *surface2;
	SDL_Color color;
	SDL_Rect src;
	SDL_Rect title;

	void pushAnyKey(void);
	void initInterfaces(void);
	void drawCreatureStatus(Creature *creaturePtr);

public:
	void Redraw(void);
	void ViewCreatureStatus(Creature *creaturePtr);

	GameSurfaceSDL(void);
	~GameSurfaceSDL(void);
	GameCommand GetCommand(void);
};
