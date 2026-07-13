#ifndef CONSTANTS_H
#define CONSTANTS_H

/* ============================================================================
 * CONFIGURACAO DE AMBIENTE (CPULator vs Placa)
 * ============================================================================
 * Descomente a linha abaixo para rodar no CPUlator.
 * Deixe comentada para rodar na Placa DE1-SoC real.
 */
#define CPULATOR

/* ============================================================================
 * ENDERECOS DE HARDWARE (memory-mapped I/O) - DE1-SoC / CPUlator
 * ============================================================================
 */

#define VGA_BUFFER_CTRL    0xFF203020
#define VGA_BACKBUFFER_CTRL 0xFF203024
#define VGA_RESOLUTION      0xFF203028
#define VGA_STATUS          0xFF20302C

#define FRONT_BUFFER_ADDR  0xC8000000
#define BACK_BUFFER_ADDR   0xC0000000

#define HEX3_HEX0_BASE     0xFF200020
#define HEX5_HEX4_BASE     0xFF200030

#define LEDR_BASE          0xFF200000
#define SW_BASE            0xFF200040
#define KEY_BASE           0xFF200050
#define PS2_BASE           0xFF200100

/* ============================================================================
 * CONSTANTES DO JOGO
 * ============================================================================
 */

#define SCREEN_WIDTH   320
#define SCREEN_HEIGHT  240

#define GROUND_Y          200
#define GRAVITY            1
#define JUMP_VELOCITY     -13
#define MOVE_SPEED          4
#define MAX_FALL_SPEED      9

#define PLAYER_W           24
#define PLAYER_H           24
#define ANIM_SPEED          4  /* frames de jogo por frame de animacao */

#define MAX_INIMIGOS       10
#define INIMIGO_W           14
#define INIMIGO_H           18
#define INIMIGO_VELOCIDADE_PATRULHA      1
#define INIMIGO_VELOCIDADE_PERSEGUICAO   2
#define DISTANCIA_DETECCAO 90
#define COOLDOWN_TIRO_INIMIGO 80

#define CHEFE_W             28
#define CHEFE_H             34
#define CHEFE_VIDA_MAX       5
#define CHEFE_X           1420

#define MAX_PROJETEIS      10
#define PROJETIL_SPEED       6
#define PROJETIL_W           6
#define PROJETIL_H           2
#define PROJETIL_ALCANCE    80

#define MAX_GAPS             5
#define MAX_LAVAS            4
#define MAX_PLATAFORMAS      6
#define MAX_VIDAS            4

#define INVULNERABLE_FRAMES 50

#define PONTUACAO_MINIMA   500
#define BANDEIRA_X        1470

#define MAX_TOKENS         10
#define TOKEN_W            16
#define TOKEN_H            16
#define TOKENS_PARA_VENCER  8

/* ============================================================================
 * CORES (RGB565)
 * ============================================================================
 */
#define COLOR_BLACK       0x0000
#define COLOR_WHITE       0xFFFF
#define COLOR_SKY         0x4D9F
#define COLOR_SAND        0xEDA9
#define COLOR_STONE       0x7BCF
#define COLOR_STONE_DARK  0x528A
#define COLOR_PLAYER_SKIN 0xFD8A
#define COLOR_PLAYER_SHIRT 0x07E0 /* Verde claro (teste) */
#define COLOR_PLAYER_HAT  0x9B26
#define COLOR_ENEMY       0x4208
#define COLOR_ENEMY_EYE   0xF800
#define COLOR_ENEMY_ALERTA 0xFB20
#define COLOR_GAP         0x0000
#define COLOR_WHIP        0xFFE0
#define COLOR_TREASURE    0xFFE0
#define COLOR_PLATFORM    0x7BCF
#define COLOR_PLATFORM_TOP 0xFFE0
#define COLOR_FLAG_POLE   0xC618
#define COLOR_FLAG        0xF800
#define COLOR_LAVA        0xF800
#define COLOR_LAVA_GLOW   0xFBE0
#define COLOR_PROJ_INIMIGO 0xF81F
#define COLOR_CHEFE       0x6000
#define COLOR_CHEFE_VIDA  0x07E0
#define COLOR_CHEFE_VIDA_FUNDO 0xF800
#define COLOR_TOKEN       0xFFE0
#define COLOR_TOKEN_BRILHO 0xFFFF

#define COLOR_SKY_ZONA1    0x4D9F
#define COLOR_SAND_ZONA1   0xEDA9
#define COLOR_SKY_ZONA2    0x32B6
#define COLOR_SAND_ZONA2   0xB965
#define COLOR_SKY_ZONA3    0x1A4F
#define COLOR_SAND_ZONA3   0x8C4A

/* ============================================================================
 * SCANCODES PS/2
 * ============================================================================
 */
#define SCANCODE_A      0x1C
#define SCANCODE_D      0x23
#define SCANCODE_W      0x1D
#define SCANCODE_J      0x3B
#define SCANCODE_SPACE  0x29
#define SCANCODE_R      0x2D
#define SCANCODE_LEFT   0x6B
#define SCANCODE_RIGHT  0x74
#define SCANCODE_UP     0x75

#endif
