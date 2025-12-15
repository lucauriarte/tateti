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

/* Mapeo de posiciones (fila, columna) a códigos de tecla */
static const Keyboard_Key_t key_map[KEYBOARD_ROWS][KEYBOARD_COLS] = {
    {KEY_1, KEY_2, KEY_3, KEY_A},
    {KEY_4, KEY_5, KEY_6, KEY_B},
    {KEY_7, KEY_8, KEY_9, KEY_C},
    {KEY_ASTERISK, KEY_0, KEY_HASH, KEY_D}
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
        case KEY_0: return "0";
        case KEY_1: return "1";
        case KEY_2: return "2";
        case KEY_3: return "3";
        case KEY_4: return "4";
        case KEY_5: return "5";
        case KEY_6: return "6";
        case KEY_7: return "7";
        case KEY_8: return "8";
        case KEY_9: return "9";
        case KEY_A: return "A";
        case KEY_B: return "B";
        case KEY_C: return "C";
        case KEY_D: return "D";
        case KEY_ASTERISK: return "*";
        case KEY_HASH: return "#";
        default: return "NONE";
    }
}
