/**
 ******************************************************************************
 * @file    display.h
 * @brief   Módulo de display para matriz de LEDs WS2812B
 *          Mapea el estado del juego a la matriz de 16 LEDs
 ******************************************************************************
 */

#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_

#include <stdint.h>
#include <stdbool.h>
#include "game_logic.h"
#include "ws2812b.h"

/* Funciones públicas */
void Display_Init(void);
void Display_Clear(void);
void Display_SetPlayer1Color(WS2812B_Color_t color);
void Display_SetPlayer2Color(WS2812B_Color_t color);
void Display_UpdateBoard(CellState_t board[9]);
void Display_ShowScores(uint8_t p1_score, uint8_t p2_score);
void Display_ShowTurn(CellState_t current_player);
void Display_MatchWinAnimation(WinType_t win_type, CellState_t winner);
void Display_GameWinAnimation(CellState_t winner);
void Display_Update(void);
void Display_UpdateAll(uint8_t p1_score, uint8_t p2_score, CellState_t current_player);
void Display_ShowColorSelection(void);

#endif /* INC_DISPLAY_H_ */
