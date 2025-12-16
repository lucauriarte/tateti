/**
 * @file tateti_glue.c
 * @brief Implementación de operaciones requeridas por el statechart
 * 
 * Este archivo mapea las operaciones abstractas definidas en el statechart
 * a las funciones reales de implementación de los módulos del juego.
 */

#include "tateti_required.h"
#include "game_logic.h"
#include "display.h"
#include "game_input.h"
#include "color_manager.h"

/* Operaciones de tablero/juego */
void tateti_init_board(Tateti* handle)
{
    (void)handle;
    Game_Init();
}

void tateti_reset_board(Tateti* handle)
{
    (void)handle;
    Game_Reset();
}

sc_boolean tateti_is_valid_move(Tateti* handle, const sc_integer position)
{
    (void)handle;
    return (sc_boolean)Game_IsValidMove((uint8_t)position);
}

void tateti_make_move(Tateti* handle, const sc_integer position, const sc_integer player)
{
    (void)handle;
    Game_MakeMove((uint8_t)position, (uint8_t)player);
}

sc_integer tateti_check_win(Tateti* handle)
{
    (void)handle;
    return (sc_integer)Game_CheckWin();
}

sc_boolean tateti_check_draw(Tateti* handle)
{
    (void)handle;
    return (sc_boolean)Game_CheckDraw();
}

/* Operaciones de display */
void tateti_update_display(Tateti* handle, const sc_integer p1, const sc_integer p2, const sc_integer player)
{
    (void)handle;
    Display_UpdateAll((uint8_t)p1, (uint8_t)p2, (uint8_t)player);
}

void tateti_show_match_win(Tateti* handle, const sc_integer win_type, const sc_integer winner)
{
    (void)handle;
    Display_MatchWinAnimation((WinType_t)win_type, (CellState_t)winner);
}

void tateti_show_game_win(Tateti* handle, const sc_integer winner)
{
    (void)handle;
    Display_GameWinAnimation((CellState_t)winner);
}

/* Operaciones de selección de color */
void tateti_cycle_color_p1(Tateti* handle)
{
    (void)handle;
    ColorManager_CyclePlayer1();
}

void tateti_cycle_color_p2(Tateti* handle)
{
    (void)handle;
    ColorManager_CyclePlayer2();
}

void tateti_show_color_selection(Tateti* handle)
{
    (void)handle;
    Display_ShowColorSelection();
}

/* Operaciones de entrada */
sc_boolean tateti_is_board_key(Tateti* handle, const sc_integer key)
{
    (void)handle;
    return (sc_boolean)GameInput_IsBoardAction((uint8_t)key);
}

sc_boolean tateti_is_color_p1_key(Tateti* handle, const sc_integer key)
{
    (void)handle;
    return (sc_boolean)GameInput_IsColorP1Action((uint8_t)key);
}

sc_boolean tateti_is_color_p2_key(Tateti* handle, const sc_integer key)
{
    (void)handle;
    return (sc_boolean)GameInput_IsColorP2Action((uint8_t)key);
}

sc_boolean tateti_is_reset_key(Tateti* handle, const sc_integer key)
{
    (void)handle;
    return (sc_boolean)GameInput_IsResetAction((uint8_t)key);
}

sc_integer tateti_key_to_position(Tateti* handle, const sc_integer key)
{
    (void)handle;
    return (sc_integer)GameInput_KeyToPosition((uint8_t)key);
}
