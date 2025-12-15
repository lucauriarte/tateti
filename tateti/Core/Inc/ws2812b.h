/**
  ******************************************************************************
  * @file           : ws2812b.h
  * @brief          : Driver para matriz de LEDs WS2812B 4x4
  * @author         : Proyecto Tateti
  ******************************************************************************
  * @attention
  * 
  * Driver para controlar una matriz de 16 LEDs WS2812B usando PWM + DMA.
  * Configuración PROBADA: TIM4 CH1 en PD12, Period=104, 84 MHz timer clock
  * 
  * Basado en librería de ALCIDES_RAMOS.
  *
  ******************************************************************************
  */

#ifndef __WS2812B_H
#define __WS2812B_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdint.h>

/* Defines -------------------------------------------------------------------*/
#define WS2812B_NUM_LEDS    16

/* Valores PWM para bits '0' y '1' (PROBADOS Y FUNCIONANDO)
 * Para TIM4 @ 84MHz, Period=104:
 * PWM_1 = 67 (~64% duty cycle) para bit '1'
 * PWM_0 = 34 (~32% duty cycle) para bit '0'
 */
#define WS2812B_PWM_BIT1     67
#define WS2812B_PWM_BIT0     34

/* Timer configuration - ajustar según tu pin */
extern TIM_HandleTypeDef htim4;
#define WS2812B_TIMER        htim4
#define WS2812B_CHANNEL      TIM_CHANNEL_1  // Ajustar según configuración CubeMX

/* Structures ----------------------------------------------------------------*/
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} WS2812B_Color_t;

/* Function prototypes -------------------------------------------------------*/

/**
 * @brief Inicializa el driver WS2812B
 * @param htim: Handle del timer configurado (TIM4)
 * Function prototypes -------------------------------------------------------*/

/**
 * @brief Establece el color de un LED específico
 * @param led: Número de LED (0 a 15)
 * @param r: Componente rojo (0-255)
 * @param g: Componente verde (0-255)
 * @param b: Componente azul (0-255)
 */
void WS2812B_SetPixel(uint8_t led, uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Establece el color usando estructura
 * @param led: Número de LED (0 a 15)
 * @param color: Estructura con valores RGB
 */
void WS2812B_SetPixelColor(uint8_t led, WS2812B_Color_t color);

/**
 * @brief Apaga todos los LEDs
 */
void WS2812B_Clear(void);

/**
 * @brief Actualiza la matriz de LEDs enviando los datos por DMA
 */
void WS2812B_Update(void);

#ifdef __cplusplus
}
#endif

#endif /* __WS2812B_H */
