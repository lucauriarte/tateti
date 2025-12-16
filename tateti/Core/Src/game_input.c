/**
 ******************************************************************************
 * @file    game_input.c
 * @brief   Implementación del mapeo de teclas a acciones del juego
 ******************************************************************************
 */

#include "game_input.h"

/* Mapeo de teclas a posiciones del tablero según PROYECTO_TATETI.md
 * 
 * Teclado físico (rotado 90° izq):   Posiciones tablero:
 * [P3 ] [P7 ] [P11] [P15]             [CHG_P1] [CHG_P2] ---      [RESET]
 * [P2 ] [P6 ] [P10] [P14]             [2]      [5]      [8]     [8]
 * [P1 ] [P5 ] [P9 ] [P13]             [1]      [4]      [7]     [7]
 * [P0 ] [P4 ] [P8 ] [P12]             ---      [0]      [3]     [6]
 */

/**
 * @brief  Procesa una tecla y retorna la acción correspondiente
 * @param  key: Tecla presionada
 * @retval Estructura GameInput_t con la acción y datos asociados
 */
GameInput_t GameInput_ProcessKey(Keyboard_Key_t key)
{
    GameInput_t result = {ACTION_NONE, 0};
    
    switch (key) {
        // Posiciones del tablero (P4-P6, P8-P10, P12-P14)
        case KEY_P4:
            result.action = ACTION_BOARD_POSITION;
            result.board_position = 0;
            break;
        case KEY_P5:
            result.action = ACTION_BOARD_POSITION;
            result.board_position = 1;
            break;
        case KEY_P6:
            result.action = ACTION_BOARD_POSITION;
            result.board_position = 2;
            break;
        case KEY_P8:
            result.action = ACTION_BOARD_POSITION;
            result.board_position = 3;
            break;
        case KEY_P9:
            result.action = ACTION_BOARD_POSITION;
            result.board_position = 4;
            break;
        case KEY_P10:
            result.action = ACTION_BOARD_POSITION;
            result.board_position = 5;
            break;
        case KEY_P12:
            result.action = ACTION_BOARD_POSITION;
            result.board_position = 6;
            break;
        case KEY_P13:
            result.action = ACTION_BOARD_POSITION;
            result.board_position = 7;
            break;
        case KEY_P14:
            result.action = ACTION_BOARD_POSITION;
            result.board_position = 8;
            break;
            
        // Cambio de color jugador 1 (P3)
        case KEY_P3:
            result.action = ACTION_CHANGE_COLOR_P1;
            break;
            
        // Cambio de color jugador 2 (P7)
        case KEY_P7:
            result.action = ACTION_CHANGE_COLOR_P2;
            break;
            
        // Reset (P15)
        case KEY_P15:
            result.action = ACTION_RESET;
            break;
            
        // Teclas sin función (P0, P1, P2, P11)
        default:
            result.action = ACTION_NONE;
            break;
    }
    
    return result;
}

/**
 * @brief  Verifica si una tecla corresponde a una posición válida del tablero
 * @param  key: Tecla a verificar
 * @retval true si es tecla de tablero (P4-P6, P8-P10, P12-P14), false si no
 */
bool GameInput_IsValidBoardKey(Keyboard_Key_t key)
{
    return (key == KEY_P4  || key == KEY_P5  || key == KEY_P6 ||
            key == KEY_P8  || key == KEY_P9  || key == KEY_P10 ||
            key == KEY_P12 || key == KEY_P13 || key == KEY_P14);
}

/**
 * @brief  Convierte una tecla a posición del tablero
 * @param  key: Tecla presionada (debe ser válida según IsValidBoardKey)
 * @retval Posición del tablero (0-8), o 0xFF si la tecla no es válida
 */
uint8_t GameInput_KeyToBoardPosition(Keyboard_Key_t key)
{
    switch (key) {
        case KEY_P4:  return 0;
        case KEY_P5:  return 1;
        case KEY_P6:  return 2;
        case KEY_P8:  return 3;
        case KEY_P9:  return 4;
        case KEY_P10: return 5;
        case KEY_P12: return 6;
        case KEY_P13: return 7;
        case KEY_P14: return 8;
        default: return 0xFF;  // Tecla inválida
    }
}

/**
 * @brief  Verifica si la tecla es una acción de tablero
 * @param  key: Tecla presionada
 * @retval true si es acción de tablero, false en caso contrario
 */
bool GameInput_IsBoardAction(Keyboard_Key_t key)
{
    return (GameInput_ProcessKey(key).action == ACTION_BOARD_POSITION);
}

/**
 * @brief  Verifica si la tecla es cambio de color P1
 * @param  key: Tecla presionada
 * @retval true si es cambio color P1, false en caso contrario
 */
bool GameInput_IsColorP1Action(Keyboard_Key_t key)
{
    return (GameInput_ProcessKey(key).action == ACTION_CHANGE_COLOR_P1);
}

/**
 * @brief  Verifica si la tecla es cambio de color P2
 * @param  key: Tecla presionada
 * @retval true si es cambio color P2, false en caso contrario
 */
bool GameInput_IsColorP2Action(Keyboard_Key_t key)
{
    return (GameInput_ProcessKey(key).action == ACTION_CHANGE_COLOR_P2);
}

/**
 * @brief  Verifica si la tecla es reset
 * @param  key: Tecla presionada
 * @retval true si es reset, false en caso contrario
 */
bool GameInput_IsResetAction(Keyboard_Key_t key)
{
    return (GameInput_ProcessKey(key).action == ACTION_RESET);
}

/**
 * @brief  Convierte tecla a posición del tablero
 * @param  key: Tecla presionada
 * @retval Posición (0-8), o 0xFF si no es tecla del tablero
 */
uint8_t GameInput_KeyToPosition(Keyboard_Key_t key)
{
    if (GameInput_IsBoardAction(key)) {
        return GameInput_ProcessKey(key).board_position;
    }
    return 0xFF;  // Valor inválido
}
