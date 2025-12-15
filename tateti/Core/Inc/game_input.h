/**
 ******************************************************************************
 * @file    game_input.h
 * @brief   Mapeo de teclas del teclado a acciones del juego
 ******************************************************************************
 */

#ifndef INC_GAME_INPUT_H_
#define INC_GAME_INPUT_H_

#include <stdint.h>
#include <stdbool.h>
#include "keyboard.h"

/* Tipos de acción */
typedef enum {
    ACTION_NONE = 0,
    ACTION_BOARD_POSITION,    // Jugada en el tablero
    ACTION_CHANGE_COLOR_P1,   // Cambiar color jugador 1
    ACTION_CHANGE_COLOR_P2,   // Cambiar color jugador 2
    ACTION_RESET              // Reiniciar partida
} GameAction_t;

/* Estructura de acción procesada */
typedef struct {
    GameAction_t action;
    uint8_t board_position;   // Solo válido si action == ACTION_BOARD_POSITION (0-8)
} GameInput_t;

/* Funciones públicas */
GameInput_t GameInput_ProcessKey(Keyboard_Key_t key);
bool GameInput_IsValidBoardKey(Keyboard_Key_t key);
uint8_t GameInput_KeyToBoardPosition(Keyboard_Key_t key);

#endif /* INC_GAME_INPUT_H_ */
