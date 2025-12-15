/**
  ******************************************************************************
  * @file           : keyboard.h
  * @brief          : Driver para teclado matricial 4x4 con anti-rebote
  * @author         : Proyecto Tateti
  ******************************************************************************
  * @attention
  * 
 * Driver para escanear un teclado matricial 4x4.
 * - Orden físico cables teclado: C4, C3, C2, C1, F1, F2, F3, F4
 * - Orden pines CN10: PF13, PE9, PE11, PF14, PE13, PF15, PG14, PG9
 * - Columnas C1-C4 (Input con pull-down 10kΩ externas)
 * - Filas F1-F4 (Output Push-Pull)
 * - Anti-rebote: 30ms por software
 * 
 * **Advertencia**: PG14 es USART6_TX por defecto. Asegurarse de no habilitar USART6.
  * 
  * Layout del teclado:
  *   [1] [2] [3] [A]
  *   [4] [5] [6] [B]
  *   [7] [8] [9] [C]
  *   [*] [0] [#] [D]
  *
  ******************************************************************************
  */

#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include <stdint.h>

/* Defines -------------------------------------------------------------------*/
#define KEYBOARD_ROWS       4
#define KEYBOARD_COLS       4
#define KEYBOARD_DEBOUNCE_MS 30

/* Pines de filas - Orden cables: F1, F2, F3, F4 → Pines: PE13, PF15, PG14, PG9 */
#define ROW0_PIN    GPIO_PIN_13  // ROW0 = F1 (cable 5) = PE13 - CN10
#define ROW0_PORT   GPIOE
#define ROW1_PIN    GPIO_PIN_15  // ROW1 = F2 (cable 6) = PF15 - CN10
#define ROW1_PORT   GPIOF
#define ROW2_PIN    GPIO_PIN_14  // ROW2 = F3 (cable 7) = PG14 - CN10 (USART6_TX)
#define ROW2_PORT   GPIOG
#define ROW3_PIN    GPIO_PIN_9   // ROW3 = F4 (cable 8) = PG9 - CN10
#define ROW3_PORT   GPIOG

/* Pines de columnas - Orden cables: C4, C3, C2, C1 → Pines: PF13, PE9, PE11, PF14 */
#define COL0_PIN    GPIO_PIN_14  // COL0 = C1 (cable 4) = PF14 - CN10
#define COL0_PORT   GPIOF
#define COL1_PIN    GPIO_PIN_11  // COL1 = C2 (cable 3) = PE11 - CN10
#define COL1_PORT   GPIOE
#define COL2_PIN    GPIO_PIN_9   // COL2 = C3 (cable 2) = PE9 - CN10
#define COL2_PORT   GPIOE
#define COL3_PIN    GPIO_PIN_13  // COL3 = C4 (cable 1) = PF13 - CN10
#define COL3_PORT   GPIOF

/* Códigos de teclas */
typedef enum {
    KEY_NONE = 0,
    KEY_1 = 1,
    KEY_2 = 2,
    KEY_3 = 3,
    KEY_A = 4,
    KEY_4 = 5,
    KEY_5 = 6,
    KEY_6 = 7,
    KEY_B = 8,
    KEY_7 = 9,
    KEY_8 = 10,
    KEY_9 = 11,
    KEY_C = 12,
    KEY_ASTERISK = 13,
    KEY_0 = 14,
    KEY_HASH = 15,
    KEY_D = 16
} Keyboard_Key_t;

/* Function prototypes -------------------------------------------------------*/

/**
 * @brief Inicializa el driver del teclado
 * @retval HAL status
 */
HAL_StatusTypeDef Keyboard_Init(void);

/**
 * @brief Actualiza el estado del teclado (llamar cada 1ms desde timer)
 * @retval None
 */
void Keyboard_Update(void);

/**
 * @brief Obtiene la última tecla presionada
 * @retval Código de la tecla o KEY_NONE si no hay tecla presionada
 */
Keyboard_Key_t Keyboard_GetKey(void);

/**
 * @brief Verifica si hay una tecla presionada
 * @retval 1 si hay tecla presionada, 0 si no
 */
uint8_t Keyboard_HasKey(void);

/**
 * @brief Convierte un código de tecla a su representación en string
 * @param key: Código de tecla
 * @retval Puntero a string constante con el nombre de la tecla
 */
const char* Keyboard_KeyToString(Keyboard_Key_t key);

#ifdef __cplusplus
}
#endif

#endif /* __KEYBOARD_H */
