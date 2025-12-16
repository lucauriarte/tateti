/**
 ******************************************************************************
 * @file    display.c
 * @brief   Implementación del módulo de display
 ******************************************************************************
 */

#include "display.h"
#include "ws2812b.h"
#include "main.h"

/* Mapeo de posiciones lógicas del tablero a LEDs físicos WS2812B
 * 
 * Layout lógico (display):        LEDs físicos WS2812B:
 * [P1-1] [P1-2] [P1-3] [TURNO] → [LED15][LED11][LED7 ][LED3 ]
 * [  0  ] [  1  ] [  2  ] [P2-1]→ [LED14][LED10][LED6 ][LED2 ]
 * [  3  ] [  4  ] [  5  ] [P2-2]→ [LED13][LED9 ][LED5 ][LED1 ]
 * [  6  ] [  7  ] [  8  ] [P2-3]→ [LED12][LED8 ][LED4 ][LED0 ]
 */

// Mapeo de posiciones del tablero (0-8) a LEDs físicos
static const uint8_t board_to_led[9] = {
    14, 10, 6,   // Posiciones 0, 1, 2 (fila superior del tablero)
    13, 9,  5,   // Posiciones 3, 4, 5 (fila media del tablero)
    12, 8,  4    // Posiciones 6, 7, 8 (fila inferior del tablero)
};

// LEDs para indicadores de puntaje jugador 1 (fila 0, columnas 0-2)
static const uint8_t p1_score_leds[3] = {15, 11, 7};

// LEDs para indicadores de puntaje jugador 2 (columna 3, filas 1-3)
static const uint8_t p2_score_leds[3] = {2, 1, 0};

// LED para indicador de turno (fila 0, columna 3)
static const uint8_t turn_led = 3;

// Colores actuales de los jugadores
static WS2812B_Color_t player1_color = {50, 0, 0};  // Rojo por defecto
static WS2812B_Color_t player2_color = {0, 0, 50};  // Azul por defecto

/**
 * @brief  Inicializa el módulo de display
 * @param  None
 * @retval None
 */
void Display_Init(void)
{
    WS2812B_Clear();
    WS2812B_Update();
}

/**
 * @brief  Limpia toda la matriz de LEDs
 * @param  None
 * @retval None
 */
void Display_Clear(void)
{
    WS2812B_Clear();
    WS2812B_Update();
}

/**
 * @brief  Configura el color del jugador 1
 * @param  color: Color del jugador 1
 * @retval None
 */
void Display_SetPlayer1Color(WS2812B_Color_t color)
{
    player1_color = color;
}

/**
 * @brief  Configura el color del jugador 2
 * @param  color: Color del jugador 2
 * @retval None
 */
void Display_SetPlayer2Color(WS2812B_Color_t color)
{
    player2_color = color;
}

/**
 * @brief  Actualiza el tablero de juego en la matriz de LEDs
 * @param  board: Estado del tablero (9 posiciones)
 * @retval None
 */
void Display_UpdateBoard(CellState_t board[9])
{
    for (uint8_t i = 0; i < 9; i++) {
        uint8_t led = board_to_led[i];
        
        if (board[i] == CELL_PLAYER1) {
            WS2812B_SetPixel(led, player1_color.r, player1_color.g, player1_color.b);
        } else if (board[i] == CELL_PLAYER2) {
            WS2812B_SetPixel(led, player2_color.r, player2_color.g, player2_color.b);
        } else {
            WS2812B_SetPixel(led, 0, 0, 0); // Apagado
        }
    }
}

/**
 * @brief  Muestra los puntajes de ambos jugadores
 * @param  p1_score: Puntaje del jugador 1 (0-3)
 * @param  p2_score: Puntaje del jugador 2 (0-3)
 * @retval None
 */
void Display_ShowScores(uint8_t p1_score, uint8_t p2_score)
{
    // Puntaje jugador 1 (LEDs superiores)
    for (uint8_t i = 0; i < 3; i++) {
        if (i < p1_score) {
            WS2812B_SetPixel(p1_score_leds[i], player1_color.r, player1_color.g, player1_color.b);
        } else {
            WS2812B_SetPixel(p1_score_leds[i], 0, 0, 0);
        }
    }
    
    // Puntaje jugador 2 (LEDs laterales)
    for (uint8_t i = 0; i < 3; i++) {
        if (i < p2_score) {
            WS2812B_SetPixel(p2_score_leds[i], player2_color.r, player2_color.g, player2_color.b);
        } else {
            WS2812B_SetPixel(p2_score_leds[i], 0, 0, 0);
        }
    }
}

/**
 * @brief  Muestra el indicador de turno
 * @param  current_player: Jugador actual (CELL_PLAYER1 o CELL_PLAYER2)
 * @retval None
 */
void Display_ShowTurn(CellState_t current_player)
{
    if (current_player == CELL_PLAYER1) {
        WS2812B_SetPixel(turn_led, player1_color.r, player1_color.g, player1_color.b);
    } else if (current_player == CELL_PLAYER2) {
        WS2812B_SetPixel(turn_led, player2_color.r, player2_color.g, player2_color.b);
    } else {
        WS2812B_SetPixel(turn_led, 0, 0, 0);
    }
}

/**
 * @brief  Animación de victoria de partida (parpadeo de línea ganadora)
 * @param  win_type: Tipo de victoria (fila, columna o diagonal)
 * @param  winner: Jugador ganador (CELL_PLAYER1 o CELL_PLAYER2)
 * @retval None
 */
void Display_MatchWinAnimation(WinType_t win_type, CellState_t winner)
{
    WS2812B_Color_t winner_color = (winner == CELL_PLAYER1) ? player1_color : player2_color;
    uint8_t winning_leds[3];
    
    // Determinar qué LEDs forman la línea ganadora
    switch (win_type) {
        case WIN_ROW0:
            winning_leds[0] = board_to_led[0];
            winning_leds[1] = board_to_led[1];
            winning_leds[2] = board_to_led[2];
            break;
        case WIN_ROW1:
            winning_leds[0] = board_to_led[3];
            winning_leds[1] = board_to_led[4];
            winning_leds[2] = board_to_led[5];
            break;
        case WIN_ROW2:
            winning_leds[0] = board_to_led[6];
            winning_leds[1] = board_to_led[7];
            winning_leds[2] = board_to_led[8];
            break;
        case WIN_COL0:
            winning_leds[0] = board_to_led[0];
            winning_leds[1] = board_to_led[3];
            winning_leds[2] = board_to_led[6];
            break;
        case WIN_COL1:
            winning_leds[0] = board_to_led[1];
            winning_leds[1] = board_to_led[4];
            winning_leds[2] = board_to_led[7];
            break;
        case WIN_COL2:
            winning_leds[0] = board_to_led[2];
            winning_leds[1] = board_to_led[5];
            winning_leds[2] = board_to_led[8];
            break;
        case WIN_DIAG_MAIN:
            winning_leds[0] = board_to_led[0];
            winning_leds[1] = board_to_led[4];
            winning_leds[2] = board_to_led[8];
            break;
        case WIN_DIAG_ANTI:
            winning_leds[0] = board_to_led[2];
            winning_leds[1] = board_to_led[4];
            winning_leds[2] = board_to_led[6];
            break;
        default:
            return; // No hay victoria
    }
    
    // Parpadear 3 veces
    for (uint8_t blink = 0; blink < 3; blink++) {
        // Apagar línea ganadora
        for (uint8_t i = 0; i < 3; i++) {
            WS2812B_SetPixel(winning_leds[i], 0, 0, 0);
        }
        WS2812B_Update();
        HAL_Delay(300);
        
        // Encender línea ganadora
        for (uint8_t i = 0; i < 3; i++) {
            WS2812B_SetPixel(winning_leds[i], winner_color.r, winner_color.g, winner_color.b);
        }
        WS2812B_Update();
        HAL_Delay(300);
    }
}

/**
 * @brief  Animación de victoria final (toda la matriz)
 * @param  winner: Jugador ganador (CELL_PLAYER1 o CELL_PLAYER2)
 * @retval None
 */
void Display_GameWinAnimation(CellState_t winner)
{
    WS2812B_Color_t winner_color = (winner == CELL_PLAYER1) ? player1_color : player2_color;
    // Animación de barrido de toda la matriz 4x4
    for (uint8_t repeat = 0; repeat < 3; repeat++) {
        // Encender todos los LEDs
        for (uint8_t i = 0; i < 16; i++) {
            WS2812B_SetPixel(i, winner_color.r, winner_color.g, winner_color.b);
            WS2812B_Update();
            HAL_Delay(80);
        }
        
        HAL_Delay(200);
        
        // Apagar todos los LEDs
        for (uint8_t i = 0; i < 16; i++) {
            WS2812B_SetPixel(i, 0, 0, 0);
            WS2812B_Update();
            HAL_Delay(80);
        }
        
        HAL_Delay(200);
    }
    
    // Dejar toda la matriz encendida al final
    for (uint8_t i = 0; i < 16; i++) {
        WS2812B_SetPixel(i, winner_color.r, winner_color.g, winner_color.b);
    }
    WS2812B_Update();
    HAL_Delay(2000);
}

/**
 * @brief  Envía los datos a la matriz de LEDs
 * @param  None
 * @retval None
 */
void Display_Update(void)
{
    WS2812B_Update();
}

/**
 * @brief  Actualiza tablero, puntajes, turno y LEDs físicamente
 * @param  p1_score: Puntaje jugador 1
 * @param  p2_score: Puntaje jugador 2
 * @param  current_player: Jugador actual (CELL_PLAYER1 o CELL_PLAYER2)
 * @retval None
 */
void Display_UpdateAll(uint8_t p1_score, uint8_t p2_score, CellState_t current_player)
{
    CellState_t board[9];
    Game_GetBoard(board);
    Display_UpdateBoard(board);
    Display_ShowScores(p1_score, p2_score);
    Display_ShowTurn(current_player);
    Display_Update();
}

/**
 * @brief  Muestra visualización de selección de colores
 *         Divide el tablero en dos mitades para mostrar ambos colores
 * @retval None
 */
void Display_ShowColorSelection(void)
{
    // Mitad superior del tablero (LEDs 0-4): Color P1
    for (uint8_t i = 0; i < 5; i++) {
        WS2812B_SetPixelColor(board_to_led[i], player1_color);
    }
    
    // Mitad inferior del tablero (LEDs 5-8): Color P2
    for (uint8_t i = 5; i < 9; i++) {
        WS2812B_SetPixelColor(board_to_led[i], player2_color);
    }
    
    // LEDs de scores y turno apagados durante selección
    WS2812B_Color_t off = {0, 0, 0};
    for (uint8_t i = 0; i < 3; i++) {
        WS2812B_SetPixelColor(p1_score_leds[i], off);
        WS2812B_SetPixelColor(p2_score_leds[i], off);
    }
    WS2812B_SetPixelColor(turn_led, off);
    
    WS2812B_Update();
}
