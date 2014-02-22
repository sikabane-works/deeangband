/*!
 * @file GameSurfaceSDL.h
 * @brief GameSurfaceSDL�N���X�Ƃ��̑��t���v�f�̃v���g�^�C�v
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
 * @class GameSurfaceSDL
 * @brief �Q�[���̃��C���C���^�[�t�F�C�X�N���X(SDL����)
 */
class GameSurfaceSDL : public GameSurface
{
private:
	SDL_Renderer *renderer; 
	SDL_RWops *rwop;
	string error;
	SDL_Window *window;
	TTF_Font* font;
	SDL_Surface *surface, *surface2;
	SDL_Color color;
	SDL_Rect src;
	SDL_Rect title;

	void initInterfaces(void);
	bool SDL_event(void);

public:
	void Redraw(void);
	GameSurfaceSDL(void);
	~GameSurfaceSDL(void);
	GameCommand GetCommand(void);
};
