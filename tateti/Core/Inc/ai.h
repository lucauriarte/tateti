/**
 ******************************************************************************
 * @file    ai.h
 * @brief   Módulo de inteligencia artificial para tateti
 ******************************************************************************
 */

#ifndef INC_AI_H_
#define INC_AI_H_

#include <stdint.h>
#include "keyboard.h"

/**
 * @brief Niveles de dificultad de la IA
 */
typedef enum {
    AI_EASY = 0,    // Movimientos aleatorios
    AI_MEDIUM = 1,  // Bloquea y busca ganar
    AI_HARD = 2     // Minimax (invencible)
} AI_Difficulty_t;

/**
 * @brief  Calcula el siguiente movimiento de la IA según nivel configurado
 * @retval Tecla correspondiente al movimiento (KEY_P4 a KEY_P14)
 */
Keyboard_Key_t AI_CalculateMove(void);

/**
 * @brief  Configura el nivel de dificultad de la IA
 * @param  difficulty: Nivel de dificultad (AI_EASY, AI_MEDIUM, AI_HARD)
 */
void AI_SetDifficulty(AI_Difficulty_t difficulty);

/**
 * @brief  Obtiene el nivel de dificultad actual
 * @retval Nivel de dificultad actual
 */
AI_Difficulty_t AI_GetDifficulty(void);

#endif /* INC_AI_H_ */
