#pragma once

#include "resource.h"

#define GAME_TITLE "D'angband"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 480

/*!
 * ゲームコマンド定義
 */
typedef enum GameCommand 
{
	GAME_COMMAND_REDRAW,
	GAME_COMMAND_EXIT
} GameCommand;

/*!
 * プレイ終了コード
 */
typedef enum PlayExitCode
{
	PLAY_EXIT_DEAD,
	PLAY_EXIT_QUIT
} PlayExitCode;
