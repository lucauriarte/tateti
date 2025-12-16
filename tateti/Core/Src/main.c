/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "eth.h"
#include "tim.h"
#include "usart.h"
#include "usb_otg.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "tateti.h"
#include "game_logic.h"
#include "display.h"
#include "ws2812b.h"
#include "keyboard.h"
#include "game_input.h"
#include "color_manager.h"
#include "ai.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static Tateti statechart_handle;
static uint8_t game_mode = 0;  // 0=PvP, 1=PvIA

// Getter para game_mode
uint8_t GetGameMode(void) {
    return game_mode;
}
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ETH_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_TIM6_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  // Inicializar módulos
  Display_Init();
  Keyboard_Init();
  ColorManager_Init();
  
  // Inicializar y activar statechart
  tateti_init(&statechart_handle);
  tateti_enter(&statechart_handle);
  
  // Iniciar timer para teclado
  HAL_TIM_Base_Start_IT(&htim6);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    // Procesar completion transitions (sin evento)
    // Esto permite que estados como Check_win y Match_end fluyan automáticamente
    tateti_trigger_without_event(&statechart_handle);
    
    // Verificar si hay tecla presionada
    if (Keyboard_HasKey()) {
        Keyboard_Key_t key = Keyboard_GetKey();
        
        // Si estamos en IDLE, procesar teclas especiales
        if (tateti_is_state_active(&statechart_handle, Tateti_main_region_Idle)) {
            if (key == KEY_P11) {
                // P11: Toggle modo de juego (PvP ↔ PvIA)
                game_mode = !game_mode;
                Display_ShowGameMode(game_mode);
                
                // Mostrar nivel de dificultad si se activa modo IA
                if (game_mode == 1) {
                    Display_ShowAIDifficulty(AI_GetDifficulty());
                }
                
                HAL_Delay(500);
                Display_ShowColorSelection();
            } else if (key == KEY_P0 && game_mode == 1) {
                // P0: Dificultad Fácil (solo en modo IA) - Verde en esquinas
                AI_SetDifficulty(AI_EASY);
                Display_ShowAIDifficulty(AI_EASY);
                HAL_Delay(500);
                Display_ShowColorSelection();
            } else if (key == KEY_P1 && game_mode == 1) {
                // P1: Dificultad Media (solo en modo IA) - Naranja en esquinas
                AI_SetDifficulty(AI_MEDIUM);
                Display_ShowAIDifficulty(AI_MEDIUM);
                HAL_Delay(500);
                Display_ShowColorSelection();
            } else if (key == KEY_P2 && game_mode == 1) {
                // P2: Dificultad Difícil (solo en modo IA) - Rojo en esquinas
                AI_SetDifficulty(AI_HARD);
                Display_ShowAIDifficulty(AI_HARD);
                HAL_Delay(500);
                Display_ShowColorSelection();
            } else {
                // Otras teclas: enviar al statechart
                tateti_raise_key_pressed(&statechart_handle, (sc_integer)key);
            }
        } else {
            // Fuera de IDLE: enviar evento al statechart
            tateti_raise_key_pressed(&statechart_handle, (sc_integer)key);
        }
    }
    
    // Si modo IA y es turno de P2 en estado PLAYING, generar movimiento
    if (game_mode == 1 && 
        tateti_is_state_active(&statechart_handle, Tateti_main_region_Playing) &&
        tateti_get_current_player(&statechart_handle) == 2) {
        
        HAL_Delay(500);  // Delay para simular "pensamiento" de la IA
        Keyboard_Key_t ai_move = AI_CalculateMove();
        tateti_raise_key_pressed(&statechart_handle, (sc_integer)ai_move);
    }
  }
  /* USER CODE END 3 */
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
        Keyboard_Update();
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
