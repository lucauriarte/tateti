/**
  ******************************************************************************
  * @file           : keyboard.c
  * @brief          : Implementación del driver de teclado matricial
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "keyboard.h"

/* Private typedef -----------------------------------------------------------*/
typedef struct {
    GPIO_TypeDef* port;
    uint16_t pin;
} GPIO_Pin_t;

/* Private variables ---------------------------------------------------------*/
static const GPIO_Pin_t row_pins[KEYBOARD_ROWS] = {
    {ROW0_PORT, ROW0_PIN},
    {ROW1_PORT, ROW1_PIN},
    {ROW2_PORT, ROW2_PIN},
    {ROW3_PORT, ROW3_PIN}
};

static const GPIO_Pin_t col_pins[KEYBOARD_COLS] = {
    {COL0_PORT, COL0_PIN},
    {COL1_PORT, COL1_PIN},
    {COL2_PORT, COL2_PIN},
    {COL3_PORT, COL3_PIN}
};

/* Mapeo de posiciones (fila, columna) a códigos de tecla
 * Rotado 90° a la izquierda para coincidir con disposición física
 */
static const Keyboard_Key_t key_map[KEYBOARD_ROWS][KEYBOARD_COLS] = {
    {KEY_P3,  KEY_P7,  KEY_P11, KEY_P15},  // Fila 0
    {KEY_P2,  KEY_P6,  KEY_P10, KEY_P14},  // Fila 1
    {KEY_P1,  KEY_P5,  KEY_P9,  KEY_P13},  // Fila 2
    {KEY_P0,  KEY_P4,  KEY_P8,  KEY_P12}   // Fila 3
};

/* Variables de anti-rebote */
static uint8_t debounce_counter = 0;
static Keyboard_Key_t current_key = KEY_NONE;
static Keyboard_Key_t last_stable_key = KEY_NONE;
static uint8_t key_available = 0;

/* Private function prototypes -----------------------------------------------*/
static Keyboard_Key_t Keyboard_Scan(void);
static void Keyboard_SetRow(uint8_t row, GPIO_PinState state);
static uint8_t Keyboard_ReadColumn(uint8_t col);

/* Function implementations --------------------------------------------------*/

/**
 * @brief Inicializa el driver del teclado
 */
HAL_StatusTypeDef Keyboard_Init(void) {
    // Configurar todas las filas en LOW
    for (uint8_t i = 0; i < KEYBOARD_ROWS; i++) {
        HAL_GPIO_WritePin(row_pins[i].port, row_pins[i].pin, GPIO_PIN_RESET);
    }
    
    current_key = KEY_NONE;
    last_stable_key = KEY_NONE;
    debounce_counter = 0;
    key_available = 0;
    
    return HAL_OK;
}

/**
 * @brief Escanea el teclado una vez
 * @retval Código de tecla detectada o KEY_NONE
 */
static Keyboard_Key_t Keyboard_Scan(void) {
    // Escanear cada fila
    for (uint8_t row = 0; row < KEYBOARD_ROWS; row++) {
        // Activar la fila actual (HIGH)
        Keyboard_SetRow(row, GPIO_PIN_SET);
        
        // Pequeña espera para estabilización (2-3 ciclos de CPU)
        __NOP();
        __NOP();
        __NOP();
        
        // Leer todas las columnas
        for (uint8_t col = 0; col < KEYBOARD_COLS; col++) {
            if (Keyboard_ReadColumn(col)) {
                // Tecla detectada
                Keyboard_SetRow(row, GPIO_PIN_RESET);
                return key_map[row][col];
            }
        }
        
        // Desactivar la fila (LOW)
        Keyboard_SetRow(row, GPIO_PIN_RESET);
    }
    
    return KEY_NONE;
}

/**
 * @brief Establece el estado de una fila
 */
static void Keyboard_SetRow(uint8_t row, GPIO_PinState state) {
    if (row >= KEYBOARD_ROWS) {
        return;
    }
    HAL_GPIO_WritePin(row_pins[row].port, row_pins[row].pin, state);
}

/**
 * @brief Lee el estado de una columna
 */
static uint8_t Keyboard_ReadColumn(uint8_t col) {
    if (col >= KEYBOARD_COLS) {
        return 0;
    }
    return HAL_GPIO_ReadPin(col_pins[col].port, col_pins[col].pin) == GPIO_PIN_SET ? 1 : 0;
}

/**
 * @brief Actualiza el estado del teclado (llamar cada 1ms)
 */
void Keyboard_Update(void) {
    Keyboard_Key_t scanned_key = Keyboard_Scan();
    
    // Algoritmo de anti-rebote
    if (scanned_key == current_key) {
        // La misma tecla se mantiene, incrementar contador
        if (debounce_counter < KEYBOARD_DEBOUNCE_MS) {
            debounce_counter++;
        }
        
        // Si alcanzamos el tiempo de debounce
        if (debounce_counter == KEYBOARD_DEBOUNCE_MS) {
            // Solo generar evento si es diferente a la última tecla estable
            if (current_key != last_stable_key) {
                last_stable_key = current_key;
                
                // Solo notificar si hay una tecla presionada (no KEY_NONE repetidos)
                if (current_key != KEY_NONE) {
                    key_available = 1;
                }
            }
        }
    } else {
        // Cambió la tecla, reiniciar contador
        current_key = scanned_key;
        debounce_counter = 0;
    }
}

/**
 * @brief Obtiene la última tecla presionada
 */
Keyboard_Key_t Keyboard_GetKey(void) {
    if (key_available) {
        key_available = 0;
        return last_stable_key;
    }
    return KEY_NONE;
}

/**
 * @brief Verifica si hay una tecla disponible
 */
uint8_t Keyboard_HasKey(void) {
    return key_available;
}

/**
 * @brief Convierte código de tecla a string
 */
const char* Keyboard_KeyToString(Keyboard_Key_t key) {
    switch (key) {
        case KEY_P0:  return "P0";
        case KEY_P1:  return "P1";
        case KEY_P2:  return "P2";
        case KEY_P3:  return "P3";
        case KEY_P4:  return "P4";
        case KEY_P5:  return "P5";
        case KEY_P6:  return "P6";
        case KEY_P7:  return "P7";
        case KEY_P8:  return "P8";
        case KEY_P9:  return "P9";
        case KEY_P10: return "P10";
        case KEY_P11: return "P11";
        case KEY_P12: return "P12";
        case KEY_P13: return "P13";
        case KEY_P14: return "P14";
        case KEY_P15: return "P15";
        default: return "NONE";
    }
}
