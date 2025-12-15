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
 * Layout del teclado (matriz 4x4 de pulsadores):
 *   [P0 ] [P1 ] [P2 ] [P3 ]
 *   [P4 ] [P5 ] [P6 ] [P7 ]
 *   [P8 ] [P9 ] [P10] [P11]
 *   [P12] [P13] [P14] [P15]
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

/* Códigos de teclas - Posiciones P0 a P15 */
typedef enum {
    KEY_NONE = 0,
    KEY_P0 = 1,   // Fila 0, Col 0
    KEY_P1 = 2,   // Fila 0, Col 1
    KEY_P2 = 3,   // Fila 0, Col 2
    KEY_P3 = 4,   // Fila 0, Col 3
    KEY_P4 = 5,   // Fila 1, Col 0
    KEY_P5 = 6,   // Fila 1, Col 1
    KEY_P6 = 7,   // Fila 1, Col 2
    KEY_P7 = 8,   // Fila 1, Col 3
    KEY_P8 = 9,   // Fila 2, Col 0
    KEY_P9 = 10,  // Fila 2, Col 1
    KEY_P10 = 11, // Fila 2, Col 2
    KEY_P11 = 12, // Fila 2, Col 3
    KEY_P12 = 13, // Fila 3, Col 0
    KEY_P13 = 14, // Fila 3, Col 1
    KEY_P14 = 15, // Fila 3, Col 2
    KEY_P15 = 16  // Fila 3, Col 3
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
