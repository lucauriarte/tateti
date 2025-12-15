/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Programa de prueba: teclado matricial -> LEDs WS2812B
  * @author         : Proyecto Tateti
  ******************************************************************************
  * @attention
  *
  * Programa de prueba que mapea cada tecla del teclado matricial a un LED
  * de la matriz WS2812B. Al presionar una tecla, se enciende el LED
  * correspondiente con un color específico.
  *
  * Mapeo de teclas a LEDs (índices lógicos 0-15):
  *   Teclas 1-9  -> LEDs 0-8 (tablero 3x3)
  *   Tecla 0     -> LED 9
  *   Teclas A-D  -> LEDs 12-15
  *   Teclas *,#  -> LEDs 10-11
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "tim.h"
#include "ws2812b.h"
#include "keyboard.h"

/* Private variables ---------------------------------------------------------*/
// No definir htim4, htim6, hdma_tim4_ch4 aquí - ya están en tim.c

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void Error_Handler(void);

/* Mapeo de teclas a índices de LEDs y colores */
typedef struct {
    Keyboard_Key_t key;
    uint8_t led_index;
    uint8_t r, g, b;  // RGB directo
} KeyLED_Map_t;

/* Configuración del mapeo - CORREGIDO para inversión física en columnas */
const KeyLED_Map_t key_led_map[] = {
    // Teclas numéricas 1-9 -> Posiciones del tablero (columnas invertidas)
    // Fila 0 del teclado: [1] [2] [3] [A] -> LEDs con columnas invertidas
    {KEY_1, 3,   50, 0, 0},      // Rojo
    {KEY_2, 2,   50, 25, 0},     // Naranja
    {KEY_3, 1,   50, 50, 0},     // Amarillo
    {KEY_A, 0,   50, 0, 0},      // LED indicador P1
    
    // Fila 1 del teclado: [4] [5] [6] [B]
    {KEY_4, 7,   0, 50, 0},      // Verde
    {KEY_5, 6,   0, 50, 50},     // Cyan
    {KEY_6, 5,   0, 0, 50},      // Azul
    {KEY_B, 4,   0, 0, 50},      // LED indicador P2
    
    // Fila 2 del teclado: [7] [8] [9] [C]
    {KEY_7, 11,  25, 0, 50},     // Violeta
    {KEY_8, 10,  50, 0, 50},     // Magenta
    {KEY_9, 9,   50, 50, 50},    // Blanco
    {KEY_C, 8,   0, 50, 0},      // Verde
    
    // Fila 3 del teclado: [*] [0] [#] [D]
    {KEY_ASTERISK, 15, 50, 50, 50}, // Blanco (reset)
    {KEY_0, 14,  10, 10, 10},    // Gris
    {KEY_HASH, 13, 25, 25, 0},   // Amarillo oscuro
    {KEY_D, 12,  50, 50, 0}      // Amarillo
};

#define KEY_LED_MAP_SIZE (sizeof(key_led_map) / sizeof(KeyLED_Map_t))

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_TIM4_Init();
    MX_TIM6_Init();

    /* Initialize keyboard driver */
    if (Keyboard_Init() != HAL_OK) {
        Error_Handler();
    }

    /* Clear all LEDs */
    WS2812B_Clear();
    WS2812B_Update();

    /* Start timer for keyboard scanning (1ms interrupt) */
    HAL_TIM_Base_Start_IT(&htim6);

    /* Variables for demo */
    uint8_t current_led = 0;
    uint32_t last_blink = HAL_GetTick();

    /* Infinite loop */
    while (1)
    {
        // Verificar si hay una tecla presionada
        if (Keyboard_HasKey()) {
            Keyboard_Key_t key = Keyboard_GetKey();

            // Buscar el LED correspondiente a la tecla
            for (uint8_t i = 0; i < KEY_LED_MAP_SIZE; i++) {
                if (key_led_map[i].key == key) {
                    // Apagar el LED anterior
                    WS2812B_SetPixel(current_led, 0, 0, 0);

                    // Encender el nuevo LED
                    current_led = key_led_map[i].led_index;
                    WS2812B_SetPixel(current_led,
                                    key_led_map[i].r,
                                    key_led_map[i].g,
                                    key_led_map[i].b);

                    // Actualizar la matriz
                    WS2812B_Update();

                    break;
                }
            }
        }

        // Parpadeo del LED onboard para indicar que el programa está corriendo
        if (HAL_GetTick() - last_blink > 500) {
            HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);  // LED verde onboard
            last_blink = HAL_GetTick();
        }
    }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure. */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 4;   // HSE = 8 MHz (Nucleo-144 con ST-Link)
    RCC_OscInitStruct.PLL.PLLN = 168; // VCO = 8MHz/4 * 168 = 336 MHz
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2; // SYSCLK = 336MHz/2 = 168 MHz
    RCC_OscInitStruct.PLL.PLLQ = 7;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;   // AHB = 168 MHz
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;    // APB1 = 42 MHz
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;    // APB2 = 84 MHz

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();

    /* Configure GPIO pin Output Level (LED onboard y filas del teclado) */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);  // LED onboard
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, GPIO_PIN_RESET); // Fila 0 (F1)
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_15, GPIO_PIN_RESET); // Fila 1 (F2)
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14|GPIO_PIN_9, GPIO_PIN_RESET); // Filas 2,3 (F3,F4)

    /* Configure GPIO pin : LED onboard PB0 */
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Configure GPIO pin : Keyboard Row 0 (PE13=F1) */
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    /* Configure GPIO pin : Keyboard Row 1 (PF15=F2) */
    GPIO_InitStruct.Pin = GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    /* Configure GPIO pins : Keyboard Rows 2,3 (PG14=F3, PG9=F4) */
    GPIO_InitStruct.Pin = GPIO_PIN_14|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    /* Configure GPIO pins : Keyboard Columns (PF14=C1, PF13=C4) */
    GPIO_InitStruct.Pin = GPIO_PIN_14|GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;  // Pull-down externas de 10kΩ
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    /* Configure GPIO pins : Keyboard Columns (PE11=C2, PE9=C3) */
    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;  // Pull-down externas de 10kΩ
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called when TIM6 interrupt took place
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM6) {
        // Actualizar el estado del teclado cada 1ms
        Keyboard_Update();
    }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
        // Parpadeo rápido del LED para indicar error
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
        for (volatile uint32_t i = 0; i < 1000000; i++);
    }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}
#endif /* USE_FULL_ASSERT */
