/**
 ******************************************************************************
 * @file    color_manager.h
 * @brief   Gestión de paleta de colores y colores de jugadores
 ******************************************************************************
 */

#ifndef INC_COLOR_MANAGER_H_
#define INC_COLOR_MANAGER_H_

#include <stdint.h>
#include "ws2812b.h"

/* Funciones públicas */
void ColorManager_Init(void);
void ColorManager_CyclePlayer1(void);
void ColorManager_CyclePlayer2(void);
WS2812B_Color_t ColorManager_GetPlayer1Color(void);
WS2812B_Color_t ColorManager_GetPlayer2Color(void);

#endif /* INC_COLOR_MANAGER_H_ */
