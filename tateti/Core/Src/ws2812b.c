/**
  ******************************************************************************
  * @file           : ws2812b.c
  * @brief          : Implementación del driver WS2812B
  * @note           : Basado en librería de ALCIDES_RAMOS
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "ws2812b.h"
#include <string.h>

/* Private variables ---------------------------------------------------------*/
// Matriz para almacenar datos RGB de cada LED [LED][G, R, B]
static uint8_t LED_RGB_Color[WS2812B_NUM_LEDS][3];

// Tamaño del buffer PWM: 1 inicial + (24 bits × 16 LEDs) + 41 final para reset >50us
#define PWM_BUFFER_SIZE (1 + (24 * WS2812B_NUM_LEDS) + 41)
static uint16_t PWM_Buffer[PWM_BUFFER_SIZE];

/* Private function prototypes -----------------------------------------------*/
static void WS2812B_PrepareBuffer(void);

/* Function implementations --------------------------------------------------*/

/**
 * @brief Establece el color de un LED específico
 * @param led: Número de LED (0 a 15)
 * @param r: Componente rojo (0-255)
 * @param g: Componente verde (0-255)
 * @param b: Componente azul (0-255)
 */
void WS2812B_SetPixel(uint8_t led, uint8_t r, uint8_t g, uint8_t b)
{
    if (led >= WS2812B_NUM_LEDS) {
        return;
    }
    
    // WS2812B usa orden GRB
    LED_RGB_Color[led][0] = g;  // Verde
    LED_RGB_Color[led][1] = r;  // Rojo
    LED_RGB_Color[led][2] = b;  // Azul
}

/**
 * @brief Establece el color usando estructura
 */
void WS2812B_SetPixelColor(uint8_t led, WS2812B_Color_t color)
{
    WS2812B_SetPixel(led, color.r, color.g, color.b);
}

/**
 * @brief Apaga todos los LEDs
 */
void WS2812B_Clear(void)
{
    memset(LED_RGB_Color, 0, sizeof(LED_RGB_Color));
}

/**
 * @brief Prepara el buffer PWM a partir de los colores RGB
 */
static void WS2812B_PrepareBuffer(void)
{
    uint32_t buffer_idx = 1;  // Empezamos en 1, el [0] será 0
    
    // Para cada LED
    for (uint16_t led = 0; led < WS2812B_NUM_LEDS; led++)
    {
        // Combinar GRB en un uint32_t
        uint32_t color = ((uint32_t)LED_RGB_Color[led][0] << 16) |  // Verde
                        ((uint32_t)LED_RGB_Color[led][1] << 8) |    // Rojo
                        LED_RGB_Color[led][2];                       // Azul

        // Para cada bit (del más significativo al menos)
        for (int8_t bit = 23; bit >= 0; bit--)
        {
            if (color & ((uint32_t)1 << bit)) {
                PWM_Buffer[buffer_idx] = WS2812B_PWM_BIT1;  // Bit '1'
            } else {
                PWM_Buffer[buffer_idx] = WS2812B_PWM_BIT0;  // Bit '0'
            }
            buffer_idx++;
        }
    }

    // Añadir 41 ceros al final para reset (>50us)
    for (uint8_t i = 0; i < 41; i++) {
        PWM_Buffer[buffer_idx++] = 0;
    }
    
    // Colocar 0 al inicio
    PWM_Buffer[0] = 0;
}

/**
 * @brief Actualiza la matriz de LEDs enviando la trama por DMA
 */
void WS2812B_Update(void)
{
    WS2812B_PrepareBuffer();
    HAL_TIM_PWM_Start_DMA(&WS2812B_TIMER, WS2812B_CHANNEL,
                         (uint32_t*)PWM_Buffer, PWM_BUFFER_SIZE);
}
