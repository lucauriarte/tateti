/**
 ******************************************************************************
 * @file    game_logic.c
 * @brief   Implementación de la lógica del juego de Tateti
 ******************************************************************************
 */

#include "game_logic.h"
#include <string.h>

/* Variables privadas */
static CellState_t board[9];

/* Combinaciones ganadoras: [fila][columna] */
static const uint8_t win_combinations[8][3] = {
    {0, 1, 2},  // Fila 0
    {3, 4, 5},  // Fila 1
    {6, 7, 8},  // Fila 2
    {0, 3, 6},  // Columna 0
    {1, 4, 7},  // Columna 1
    {2, 5, 8},  // Columna 2
    {0, 4, 8},  // Diagonal principal
    {2, 4, 6}   // Diagonal anti
};

/**
 * @brief  Inicializa el tablero del juego
 * @param  None
 * @retval None
 */
void Game_Init(void)
{
    memset(board, CELL_EMPTY, sizeof(board));
}

/**
 * @brief  Resetea el tablero (equivalente a Init)
 * @param  None
 * @retval None
 */
void Game_Reset(void)
{
    Game_Init();
}

/**
 * @brief  Verifica si un movimiento es válido
 * @param  position: Posición del tablero (0-8)
 * @retval true si la posición está vacía y es válida, false en caso contrario
 */
bool Game_IsValidMove(uint8_t position)
{
    if (position > 8) {
        return false;
    }
    return (board[position] == CELL_EMPTY);
}

/**
 * @brief  Realiza un movimiento en el tablero
 * @param  position: Posición del tablero (0-8)
 * @param  player: CELL_PLAYER1 o CELL_PLAYER2
 * @retval None
 */
void Game_MakeMove(uint8_t position, CellState_t player)
{
    if (position <= 8 && player != CELL_EMPTY) {
        board[position] = player;
    }
}

/**
 * @brief  Verifica si hay un ganador
 * @param  None
 * @retval WIN_NONE si no hay ganador, o el tipo de victoria (WIN_ROW0, etc.)
 */
WinType_t Game_CheckWin(void)
{
    for (uint8_t i = 0; i < 8; i++) {
        uint8_t pos0 = win_combinations[i][0];
        uint8_t pos1 = win_combinations[i][1];
        uint8_t pos2 = win_combinations[i][2];
        
        if (board[pos0] != CELL_EMPTY &&
            board[pos0] == board[pos1] &&
            board[pos1] == board[pos2]) {
            // Retornar el tipo de victoria (1-8)
            return (WinType_t)(i + 1);
        }
    }
    return WIN_NONE;
}

/**
 * @brief  Verifica si hay empate (tablero lleno sin ganador)
 * @param  None
 * @retval true si hay empate, false en caso contrario
 */
bool Game_CheckDraw(void)
{
    // Si hay ganador, no puede haber empate
    if (Game_CheckWin() != WIN_NONE) {
        return false;
    }
    
    // Verificar si todas las celdas están ocupadas
    for (uint8_t i = 0; i < 9; i++) {
        if (board[i] == CELL_EMPTY) {
            return false;
        }
    }
    
    return true;
}

/**
 * @brief  Obtiene el estado de una celda específica
 * @param  position: Posición del tablero (0-8)
 * @retval Estado de la celda (CELL_EMPTY, CELL_PLAYER1, CELL_PLAYER2)
 */
CellState_t Game_GetCell(uint8_t position)
{
    if (position > 8) {
        return CELL_EMPTY;
    }
    return board[position];
}

/**
 * @brief  Copia el tablero completo a un array externo
 * @param  board: Array de destino [9]
 * @retval None
 */
void Game_GetBoard(CellState_t board_out[9])
{
    memcpy(board_out, board, sizeof(board));
}
