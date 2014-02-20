/*!
 * @file GameSurfaceSDL.h
 * @brief GameSurfaceSDLクラスとその他付随要素のプロトタイプ
 * @date 2014/02/19
 * @author Deskull
 * 2014 Sikabane Works.
 */

#pragma once

#include "stdafx.h"
#include "Floor.h"

#include "GameSurface.h"
#include "Deeangband.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

/*!
 * @brief SDLのキーコード定義
 */
enum SDL_KEYCODE_LIST
{
	SDL_KEY_ESC = 27,
};


/*! 
 * @class GameSurfaceSDL
 * @brief ゲームのメインインターフェイスクラス(SDL実装)
 */
class GameSurfaceSDL : public GameSurface
{
private:
	SDL_Renderer *renderer; 
	SDL_RWops *rwop;
	string error;
	SDL_Window *window;
	bool SDL_event(void);
public:
	GameSurfaceSDL(void);
	~GameSurfaceSDL(void);
	void test(void);
};
