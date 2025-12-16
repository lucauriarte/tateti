/**
 ******************************************************************************
 * @file    ai.c
 * @brief   Implementación de IA para tateti con niveles de dificultad
 ******************************************************************************
 */

#include "ai.h"
#include "game_logic.h"
#include <stdlib.h>

/* Variable privada para nivel de dificultad */
static AI_Difficulty_t ai_difficulty = AI_MEDIUM;

/* Mapeo de posición de tablero a tecla física */
static const Keyboard_Key_t position_to_key[9] = {
    KEY_P4,  // Posición 0
    KEY_P5,  // Posición 1
    KEY_P6,  // Posición 2
    KEY_P8,  // Posición 3
    KEY_P9,  // Posición 4
    KEY_P10, // Posición 5
    KEY_P12, // Posición 6
    KEY_P13, // Posición 7
    KEY_P14  // Posición 8
};

/* Prototipos funciones privadas */
static uint8_t AI_EasyMove(CellState_t* board);
static uint8_t AI_MediumMove(CellState_t* board);
static uint8_t AI_HardMove(CellState_t* board);
static uint8_t FindEmptyPosition(CellState_t* board);
static int8_t CheckWinningMove(CellState_t* board, CellState_t player);
static int8_t Minimax(CellState_t* board, uint8_t depth, bool isMaximizing);
static int8_t EvaluateBoard(CellState_t* board);

/**
 * @brief  Configura el nivel de dificultad de la IA
 */
void AI_SetDifficulty(AI_Difficulty_t difficulty)
{
    ai_difficulty = difficulty;
}

/**
 * @brief  Obtiene el nivel de dificultad actual
 */
AI_Difficulty_t AI_GetDifficulty(void)
{
    return ai_difficulty;
}

/**
 * @brief  Calcula el siguiente movimiento de la IA según nivel configurado
 */
Keyboard_Key_t AI_CalculateMove(void)
{
    CellState_t board[9];
    Game_GetBoard(board);
    
    uint8_t position;
    
    switch (ai_difficulty) {
        case AI_EASY:
            position = AI_EasyMove(board);
            break;
        case AI_MEDIUM:
            position = AI_MediumMove(board);
            break;
        case AI_HARD:
            position = AI_HardMove(board);
            break;
        default:
            position = FindEmptyPosition(board);
            break;
    }
    
    return position_to_key[position];
}

/**
 * @brief  IA Fácil - Movimiento aleatorio
 */
static uint8_t AI_EasyMove(CellState_t* board)
{
    uint8_t empty_positions[9];
    uint8_t count = 0;
    
    // Recopilar todas las posiciones vacías
    for (uint8_t i = 0; i < 9; i++) {
        if (board[i] == CELL_EMPTY) {
            empty_positions[count++] = i;
        }
    }
    
    if (count == 0) return 0;
    
    // Seleccionar aleatoriamente
    return empty_positions[rand() % count];
}

/**
 * @brief  IA Media - Bloquea y busca ganar
 */
static uint8_t AI_MediumMove(CellState_t* board)
{
    int8_t move;
    
    // 1. Intentar ganar
    move = CheckWinningMove(board, CELL_PLAYER2);
    if (move >= 0) return (uint8_t)move;
    
    // 2. Bloquear victoria del oponente
    move = CheckWinningMove(board, CELL_PLAYER1);
    if (move >= 0) return (uint8_t)move;
    
    // 3. Tomar el centro si está disponible
    if (board[4] == CELL_EMPTY) return 4;
    
    // 4. Tomar una esquina
    const uint8_t corners[4] = {0, 2, 6, 8};
    for (uint8_t i = 0; i < 4; i++) {
        if (board[corners[i]] == CELL_EMPTY) {
            return corners[i];
        }
    }
    
    // 5. Cualquier posición disponible
    return FindEmptyPosition(board);
}

/**
 * @brief  IA Difícil - Minimax (invencible)
 */
static uint8_t AI_HardMove(CellState_t* board)
{
    int8_t best_score = -100;
    uint8_t best_move = 0;
    
    for (uint8_t i = 0; i < 9; i++) {
        if (board[i] == CELL_EMPTY) {
            board[i] = CELL_PLAYER2;
            int8_t score = Minimax(board, 0, false);
            board[i] = CELL_EMPTY;
            
            if (score > best_score) {
                best_score = score;
                best_move = i;
            }
        }
    }
    
    return best_move;
}

/**
 * @brief  Encuentra primera posición vacía
 */
static uint8_t FindEmptyPosition(CellState_t* board)
{
    for (uint8_t i = 0; i < 9; i++) {
        if (board[i] == CELL_EMPTY) {
            return i;
        }
    }
    return 0;
}

/**
 * @brief  Busca un movimiento ganador para el jugador especificado
 * @retval Posición del movimiento ganador, o -1 si no hay
 */
static int8_t CheckWinningMove(CellState_t* board, CellState_t player)
{
    for (uint8_t i = 0; i < 9; i++) {
        if (board[i] == CELL_EMPTY) {
            board[i] = player;
            
            // Verificar si este movimiento gana
            WinType_t win_type = Game_CheckWin();
            bool wins = (win_type != WIN_NONE);
            
            board[i] = CELL_EMPTY;
            
            if (wins) return (int8_t)i;
        }
    }
    return -1;
}

/**
 * @brief  Algoritmo Minimax
 */
static int8_t Minimax(CellState_t* board, uint8_t depth, bool isMaximizing)
{
    int8_t score = EvaluateBoard(board);
    
    // Si el juego terminó, retornar score
    if (score != 0) return score - depth * (score > 0 ? 1 : -1);
    
    // Si es empate
    bool has_empty = false;
    for (uint8_t i = 0; i < 9; i++) {
        if (board[i] == CELL_EMPTY) {
            has_empty = true;
            break;
        }
    }
    if (!has_empty) return 0;
    
    if (isMaximizing) {
        int8_t best = -100;
        for (uint8_t i = 0; i < 9; i++) {
            if (board[i] == CELL_EMPTY) {
                board[i] = CELL_PLAYER2;
                int8_t val = Minimax(board, depth + 1, false);
                board[i] = CELL_EMPTY;
                if (val > best) best = val;
            }
        }
        return best;
    } else {
        int8_t best = 100;
        for (uint8_t i = 0; i < 9; i++) {
            if (board[i] == CELL_EMPTY) {
                board[i] = CELL_PLAYER1;
                int8_t val = Minimax(board, depth + 1, true);
                board[i] = CELL_EMPTY;
                if (val < best) best = val;
            }
        }
        return best;
    }
}

/**
 * @brief  Evalúa el estado del tablero
 * @retval +10 si P2 gana, -10 si P1 gana, 0 si no hay ganador
 */
static int8_t EvaluateBoard(CellState_t* board)
{
    // Verificar filas
    for (uint8_t i = 0; i < 3; i++) {
        if (board[i*3] != CELL_EMPTY && 
            board[i*3] == board[i*3+1] && 
            board[i*3] == board[i*3+2]) {
            return (board[i*3] == CELL_PLAYER2) ? 10 : -10;
        }
    }
    
    // Verificar columnas
    for (uint8_t i = 0; i < 3; i++) {
        if (board[i] != CELL_EMPTY && 
            board[i] == board[i+3] && 
            board[i] == board[i+6]) {
            return (board[i] == CELL_PLAYER2) ? 10 : -10;
        }
    }
    
    // Verificar diagonal principal
    if (board[0] != CELL_EMPTY && 
        board[0] == board[4] && 
        board[0] == board[8]) {
        return (board[0] == CELL_PLAYER2) ? 10 : -10;
    }
    
    // Verificar diagonal anti
    if (board[2] != CELL_EMPTY && 
        board[2] == board[4] && 
        board[2] == board[6]) {
        return (board[2] == CELL_PLAYER2) ? 10 : -10;
    }
    
    return 0;  // No hay ganador aún
}
