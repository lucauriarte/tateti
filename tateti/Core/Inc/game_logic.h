/**
 ******************************************************************************
 * @file    game_logic.h
 * @brief   Lógica del juego de Tateti (reglas, validación, detección de victoria)
 ******************************************************************************
 */

#ifndef INC_GAME_LOGIC_H_
#define INC_GAME_LOGIC_H_

#include <stdint.h>
#include <stdbool.h>

/* Tipos de dato */
typedef enum {
    CELL_EMPTY = 0,
    CELL_PLAYER1 = 1,
    CELL_PLAYER2 = 2
} CellState_t;

typedef enum {
    WIN_NONE = 0,
    WIN_ROW0 = 1,    // Fila superior (posiciones 0,1,2)
    WIN_ROW1 = 2,    // Fila media (posiciones 3,4,5)
    WIN_ROW2 = 3,    // Fila inferior (posiciones 6,7,8)
    WIN_COL0 = 4,    // Columna izquierda (posiciones 0,3,6)
    WIN_COL1 = 5,    // Columna central (posiciones 1,4,7)
    WIN_COL2 = 6,    // Columna derecha (posiciones 2,5,8)
    WIN_DIAG_MAIN = 7,   // Diagonal principal (posiciones 0,4,8)
    WIN_DIAG_ANTI = 8    // Diagonal anti (posiciones 2,4,6)
} WinType_t;

/* Funciones públicas */
void Game_Init(void);
void Game_Reset(void);
bool Game_IsValidMove(uint8_t position);
void Game_MakeMove(uint8_t position, CellState_t player);
WinType_t Game_CheckWin(void);
bool Game_CheckDraw(void);
CellState_t Game_GetCell(uint8_t position);
void Game_GetBoard(CellState_t board[9]);

#endif /* INC_GAME_LOGIC_H_ */
