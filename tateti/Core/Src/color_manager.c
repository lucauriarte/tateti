/**
 ******************************************************************************
 * @file    color_manager.c
 * @brief   Implementación de gestión de colores
 ******************************************************************************
 */

#include "color_manager.h"
#include "display.h"

/* Paleta de colores disponibles para jugadores */
/* Índices: 0=RED, 1=GREEN, 2=BLUE, 3=YELLOW, 4=CYAN, 5=MAGENTA, 6=WHITE */
static const WS2812B_Color_t color_palette[] = {
    {50, 0, 0},    // RED
    {0, 50, 0},    // GREEN
    {0, 0, 50},    // BLUE
    {50, 50, 0},   // YELLOW
    {0, 50, 50},   // CYAN
    {50, 0, 50},   // MAGENTA
    {30, 30, 30}   // WHITE
};

#define PALETTE_SIZE (sizeof(color_palette) / sizeof(WS2812B_Color_t))

/* Índices actuales de color para cada jugador */
static uint8_t player1_color_index = 0;  // RED por defecto
static uint8_t player2_color_index = 2;  // BLUE por defecto

/**
 * @brief  Inicializa el gestor de colores con colores por defecto
 * @param  None
 * @retval None
 */
void ColorManager_Init(void)
{
    player1_color_index = 0;  // RED
    player2_color_index = 2;  // BLUE
    
    // Configurar colores iniciales en el display
    Display_SetPlayer1Color(color_palette[player1_color_index]);
    Display_SetPlayer2Color(color_palette[player2_color_index]);
}

/**
 * @brief  Cambia al siguiente color para el jugador 1
 * @param  None
 * @retval None
 */
void ColorManager_CyclePlayer1(void)
{
    player1_color_index = (player1_color_index + 1) % PALETTE_SIZE;
    Display_SetPlayer1Color(color_palette[player1_color_index]);
}

/**
 * @brief  Cambia al siguiente color para el jugador 2
 * @param  None
 * @retval None
 */
void ColorManager_CyclePlayer2(void)
{
    player2_color_index = (player2_color_index + 1) % PALETTE_SIZE;
    Display_SetPlayer2Color(color_palette[player2_color_index]);
}

/**
 * @brief  Obtiene el color actual del jugador 1
 * @param  None
 * @retval Color actual del jugador 1
 */
WS2812B_Color_t ColorManager_GetPlayer1Color(void)
{
    return color_palette[player1_color_index];
}

/**
 * @brief  Obtiene el color actual del jugador 2
 * @param  None
 * @retval Color actual del jugador 2
 */
WS2812B_Color_t ColorManager_GetPlayer2Color(void)
{
    return color_palette[player2_color_index];
}
