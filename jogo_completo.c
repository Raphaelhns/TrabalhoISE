#include <stdint.h>
#include <stdlib.h>

#ifndef CONSTANTS_H
#define CONSTANTS_H

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

#define PLAYER_W           12
#define PLAYER_H           20

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
#define TOKEN_W             8
#define TOKEN_H             8
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
#ifndef TYPES_H
#define TYPES_H

typedef enum { FACING_LEFT, FACING_RIGHT } Direcao;
typedef enum { ESTADO_JOGANDO, ESTADO_PAUSADO, ESTADO_GAMEOVER, ESTADO_VITORIA } EstadoJogo;

typedef struct {
    int x, y;
    int vx, vy;
    int no_chao;
    Direcao direcao;
    int vidas;
    int pontuacao;
    int invulneravel;
    int atacando;
} Jogador;

typedef enum { COMPORTAMENTO_PARADO, COMPORTAMENTO_PATRULHA, COMPORTAMENTO_PERSEGUICAO } ComportamentoInimigo;

typedef struct {
    int x, y;
    int w, h;
    int vivo;
    int tipo;              /* 0=estatua passiva, 1=guarda nocivo, 2=atirador, 3=chefe */
    ComportamentoInimigo comportamento;
    int patrulha_min_x;
    int patrulha_max_x;
    int vx;
    Direcao direcao;
    int vida;
    int cooldown_tiro;
} Inimigo;

typedef struct {
    int x, y;
    int vx;
    int ativo;
    int dist_percorrida;
    int do_inimigo;
} Projetil;

typedef struct {
    int x;
    int largura;
} Gap;

typedef struct {
    int x;
    int largura;
} Lava;

typedef struct {
    int x, y;
    int largura;
    int altura;
} Plataforma;

typedef struct {
    int x, y;
    int coletado;
} Token;

#endif
#ifndef HARDWARE_H
#define HARDWARE_H

#include <stdint.h>

extern volatile short int *pixel_buffer_back;
extern int tecla_esquerda, tecla_direita, tecla_cima, tecla_atirar, tecla_reiniciar;

void desenha_pixel(int x, int y, short int cor);
void desenha_retangulo(int x, int y, int largura, int altura, short int cor);
void limpa_tela(short int cor);
void aguarda_swap_completo(void);
void inicializa_video(void);
void troca_buffers(void);

void processa_teclado(void);
int chave_sw0_ligada(void);

void atualiza_pontuacao_display(int pontuacao);
void atualiza_vidas_display(int vidas);
void atualiza_vidas_leds(int vidas);

#endif
#ifndef PLAYER_H
#define PLAYER_H


extern Jogador player;

void inicializa_jogador(void);
void desenha_jogador(void);
void atualiza_jogador(void);

#endif
#ifndef ENEMY_H
#define ENEMY_H


extern Inimigo inimigos[MAX_INIMIGOS];

void cria_inimigo(int indice, int x, int tipo, ComportamentoInimigo comportamento, int alcance_patrulha);
void inicializa_inimigos(void);
void desenha_inimigo(Inimigo *e);
void desenha_inimigos(void);
void move_inimigo(Inimigo *e);
void atualiza_inimigos(void);
int distancia_x(int a, int b);

#endif
#ifndef PROJECTILES_H
#define PROJECTILES_H


extern Projetil projeteis[MAX_PROJETEIS];

void inicializa_projeteis(void);
void desenha_projeteis(void);
void atualiza_projeteis(void);
void dispara_projetil_inimigo(Inimigo *e);
void dispara_projetil(void);

#endif
#ifndef LEVEL_H
#define LEVEL_H


extern Gap gaps[MAX_GAPS];
extern Lava lavas[MAX_LAVAS];
extern Plataforma plataformas[MAX_PLATAFORMAS];
extern Token tokens[MAX_TOKENS];
extern int tokens_coletados;

void inicializa_gaps(void);
void inicializa_lavas(void);
void inicializa_plataformas(void);
void inicializa_tokens(void);

int zona_visual(int mundo_x);
short int cor_ceu_zona(int zona);
short int cor_chao_zona(int zona);

void desenha_ceu(void);
void desenha_cenario(void);
void desenha_plataformas(void);
void desenha_bandeira(void);
void desenha_tokens(void);

#endif
#ifndef GAME_H
#define GAME_H


extern int camera_x;
extern int nivel_largura;
extern EstadoJogo estado;

void inicializa_jogo(void);
int colide(int ax, int ay, int aw, int ah, int bx, int by, int bw, int bh);
int esta_sobre_gap(int x, int largura);
int esta_sobre_lava(int x, int largura);
void desenha_tela_gameover(void);
void desenha_tela_vitoria(void);
void desenha_tela_pausa(void);
void renderiza_cena(void);
void atualiza_estado(void);
void processa_pausa_e_reinicio(void);
void le_entrada(void);
void atraso_frame(void);

#endif
#include <stdint.h>

volatile short int *pixel_buffer_back;
int tecla_esquerda = 0, tecla_direita = 0, tecla_cima = 0, tecla_atirar = 0;
int tecla_reiniciar = 0;

void desenha_pixel(int x, int y, short int cor) {
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return;
    volatile short int *one_pixel_address;
    one_pixel_address = pixel_buffer_back + (y << 9) + x;
    *one_pixel_address = cor;
}

void desenha_retangulo(int x, int y, int largura, int altura, short int cor) {
    int i, j;
    for (j = 0; j < altura; j++) {
        for (i = 0; i < largura; i++) {
            desenha_pixel(x + i, y + j, cor);
        }
    }
}

void limpa_tela(short int cor) {
    desenha_retangulo(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, cor);
}

void aguarda_swap_completo(void) {
    volatile int *status_ptr = (int *)VGA_STATUS;
    while ((*status_ptr & 0x01) != 0) {
    }
}

void inicializa_video(void) {
    volatile int *buffer_ctrl = (int *)VGA_BUFFER_CTRL;
    volatile int *backbuffer_ctrl = (int *)VGA_BACKBUFFER_CTRL;

    pixel_buffer_back = (volatile short int *)BACK_BUFFER_ADDR;
    limpa_tela(COLOR_SKY);

    *backbuffer_ctrl = BACK_BUFFER_ADDR;
    aguarda_swap_completo();
    *buffer_ctrl = 1;
    aguarda_swap_completo();

    pixel_buffer_back = (volatile short int *)FRONT_BUFFER_ADDR;
    limpa_tela(COLOR_SKY);
}

void troca_buffers(void) {
    volatile int *backbuffer_ctrl = (int *)VGA_BACKBUFFER_CTRL;
    volatile int *buffer_ctrl = (int *)VGA_BUFFER_CTRL;

    *backbuffer_ctrl = (int)(intptr_t)pixel_buffer_back;
    *buffer_ctrl = 1;
    aguarda_swap_completo();

    if (pixel_buffer_back == (volatile short int *)FRONT_BUFFER_ADDR) {
        pixel_buffer_back = (volatile short int *)BACK_BUFFER_ADDR;
    } else {
        pixel_buffer_back = (volatile short int *)FRONT_BUFFER_ADDR;
    }
}

void processa_teclado(void) {
    volatile int *PS2_ptr = (int *)PS2_BASE;
    int PS2_data;
    unsigned char byte_recebido;
    static int byte_quebra = 0;
    static int byte_extendido = 0;

    PS2_data = *(PS2_ptr);
    while ((PS2_data & 0x8000) != 0) {
        byte_recebido = PS2_data & 0xFF;
        if (byte_recebido == 0xF0) {
            byte_quebra = 1;
        } else if (byte_recebido == 0xE0) {
            byte_extendido = 1;
        } else {
            int pressionada = !byte_quebra;
            switch (byte_recebido) {
                case SCANCODE_A: tecla_esquerda = pressionada; break;
                case SCANCODE_D: tecla_direita = pressionada; break;
                case SCANCODE_W: tecla_cima = pressionada; break;
                case SCANCODE_J:
                case SCANCODE_SPACE:
                    if (byte_extendido == 0) tecla_atirar = pressionada;
                    break;
                case SCANCODE_R: tecla_reiniciar = pressionada; break;
                case SCANCODE_LEFT: if (byte_extendido) tecla_esquerda = pressionada; break;
                case SCANCODE_RIGHT: if (byte_extendido) tecla_direita = pressionada; break;
                case SCANCODE_UP: if (byte_extendido) tecla_cima = pressionada; break;
            }
            byte_quebra = 0;
            byte_extendido = 0;
        }
        PS2_data = *(PS2_ptr);
    }
}

int chave_sw0_ligada(void) {
    volatile int *sw_ptr = (int *)SW_BASE;
    return (*sw_ptr) & 0x1;
}

const unsigned char tabela_7seg[10] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

void atualiza_pontuacao_display(int pontuacao) {
    volatile int *hex30 = (int *)HEX3_HEX0_BASE;
    int digitos[4];
    int i;
    int valor = pontuacao;

    if (valor > 9999) valor = 9999;
    if (valor < 0) valor = 0;

    for (i = 0; i < 4; i++) {
        digitos[i] = valor % 10;
        valor /= 10;
    }

    unsigned int saida30 = tabela_7seg[digitos[0]]
                          | (tabela_7seg[digitos[1]] << 8)
                          | (tabela_7seg[digitos[2]] << 16)
                          | (tabela_7seg[digitos[3]] << 24);

    *hex30 = saida30;
}

void atualiza_vidas_display(int vidas) {
    volatile int *hex54 = (int *)HEX5_HEX4_BASE;
    unsigned int digito_vidas;

    if (vidas < 0) vidas = 0;
    if (vidas > 9) vidas = 9;

    digito_vidas = tabela_7seg[vidas];
    *hex54 = (digito_vidas << 8) | 0x00;
}

void atualiza_vidas_leds(int vidas) {
    volatile int *leds = (int *)LEDR_BASE;
    int padrao = 0;
    int i;

    if (vidas < 0) vidas = 0;
    if (vidas > MAX_VIDAS) vidas = MAX_VIDAS;

    for (i = 0; i < vidas; i++) {
        padrao |= (1 << i);
    }
    *leds = padrao;
}

Jogador player;

void inicializa_jogador(void) {
    player.x = 20;
    player.y = GROUND_Y - PLAYER_H;
    player.vx = 0;
    player.vy = 0;
    player.no_chao = 1;
    player.direcao = FACING_RIGHT;
    player.vidas = MAX_VIDAS;
    player.pontuacao = 0;
    player.invulneravel = 0;
    player.atacando = 0;
}

void desenha_jogador(void) {
    int sx = player.x - camera_x;
    int sy = player.y;

    if (player.invulneravel > 0 && (player.invulneravel % 6) < 3) {
        return;
    }

    desenha_retangulo(sx, sy + 6, PLAYER_W, PLAYER_H - 10, COLOR_PLAYER_SHIRT);
    desenha_retangulo(sx + 2, sy, PLAYER_W - 4, 6, COLOR_PLAYER_SKIN);
    desenha_retangulo(sx, sy - 3, PLAYER_W, 3, COLOR_PLAYER_HAT);
    desenha_retangulo(sx - 1, sy, PLAYER_W + 2, 2, COLOR_PLAYER_HAT);

    if (player.no_chao && player.vx != 0) {
        desenha_retangulo(sx + 1, sy + PLAYER_H - 4, 4, 4, COLOR_STONE_DARK);
        desenha_retangulo(sx + PLAYER_W - 5, sy + PLAYER_H - 4, 4, 4, COLOR_STONE_DARK);
    } else {
        desenha_retangulo(sx + 1, sy + PLAYER_H - 4, 4, 4, COLOR_STONE_DARK);
        desenha_retangulo(sx + PLAYER_W - 5, sy + PLAYER_H - 4, 4, 4, COLOR_STONE_DARK);
    }

    if (player.atacando > 0) {
        int whip_x = (player.direcao == FACING_RIGHT) ? sx + PLAYER_W : sx - 10;
        desenha_retangulo(whip_x, sy + 8, 10, 2, COLOR_WHIP);
    }
}

void atualiza_jogador(void) {
    player.vx = 0;
    if (tecla_esquerda) {
        player.vx = -MOVE_SPEED;
        player.direcao = FACING_LEFT;
    }
    if (tecla_direita) {
        player.vx = MOVE_SPEED;
        player.direcao = FACING_RIGHT;
    }

    if (tecla_cima && player.no_chao) {
        player.vy = JUMP_VELOCITY;
        player.no_chao = 0;
    }

    if (tecla_atirar && player.atacando == 0) {
        player.atacando = 10;
        dispara_projetil();
    }
    if (player.atacando > 0) {
        player.atacando--;
    }

    if (!player.no_chao) {
        player.vy += GRAVITY;
        if (player.vy > MAX_FALL_SPEED) player.vy = MAX_FALL_SPEED;
    }

    player.x += player.vx;
    player.y += player.vy;

    if (player.x < 0) player.x = 0;
    if (player.x > nivel_largura - PLAYER_W) player.x = nivel_largura - PLAYER_W;

    player.no_chao = 0;

    {
        int i;
        for (i = 0; i < MAX_PLATAFORMAS; i++) {
            int px = plataformas[i].x;
            int py = plataformas[i].y;
            int pw = plataformas[i].largura;
            if (player.x + PLAYER_W <= px || player.x >= px + pw) continue;
            if (player.vy >= 0 && player.y + PLAYER_H >= py && player.y < py) {
                player.y  = py - PLAYER_H;
                player.vy = 0;
                player.no_chao = 1;
            }
        }
    }

    if (!player.no_chao) {
        if (esta_sobre_gap(player.x, PLAYER_W)) {
            if (player.y > SCREEN_HEIGHT + 20) {
                player.vidas--;
                player.x = (player.x > 100) ? player.x - 60 : 20;
                player.y = GROUND_Y - PLAYER_H;
                player.vy = 0;
                player.invulneravel = INVULNERABLE_FRAMES;
            }
        } else if (player.y + PLAYER_H >= GROUND_Y) {
            player.y = GROUND_Y - PLAYER_H;
            player.vy = 0;
            player.no_chao = 1;

            if (esta_sobre_lava(player.x, PLAYER_W) && player.invulneravel == 0) {
                player.vidas--;
                player.vy = JUMP_VELOCITY / 2;
                player.no_chao = 0;
                player.invulneravel = INVULNERABLE_FRAMES;
            }
        }
    }

    camera_x = player.x - SCREEN_WIDTH / 2 + PLAYER_W / 2;
    if (camera_x < 0) camera_x = 0;
    if (camera_x > nivel_largura - SCREEN_WIDTH) camera_x = nivel_largura - SCREEN_WIDTH;

    if (player.invulneravel > 0) player.invulneravel--;

    {
        int i;
        for (i = 0; i < MAX_TOKENS; i++) {
            if (tokens[i].coletado) continue;
            if (colide(player.x, player.y, PLAYER_W, PLAYER_H,
                       tokens[i].x, tokens[i].y, TOKEN_W, TOKEN_H)) {
                tokens[i].coletado = 1;
                tokens_coletados++;
            }
        }
    }

    if (player.vidas <= 0) {
        estado = ESTADO_GAMEOVER;
    }

    if (player.x >= BANDEIRA_X - PLAYER_W) {
        if (tokens_coletados >= TOKENS_PARA_VENCER) {
            estado = ESTADO_VITORIA;
        } else {
            player.x = BANDEIRA_X - PLAYER_W - 1;
        }
    }
}

Inimigo inimigos[MAX_INIMIGOS];

void cria_inimigo(int indice, int x, int tipo, ComportamentoInimigo comportamento, int alcance_patrulha) {
    inimigos[indice].x = x;
    inimigos[indice].y = GROUND_Y - INIMIGO_H;
    inimigos[indice].w = (tipo == 3) ? CHEFE_W : INIMIGO_W;
    inimigos[indice].h = (tipo == 3) ? CHEFE_H : INIMIGO_H;
    inimigos[indice].y = GROUND_Y - inimigos[indice].h;
    inimigos[indice].vivo = 1;
    inimigos[indice].tipo = tipo;
    inimigos[indice].comportamento = comportamento;
    inimigos[indice].direcao = FACING_LEFT;
    inimigos[indice].vx = 0;
    inimigos[indice].vida = (tipo == 3) ? CHEFE_VIDA_MAX : 1;
    inimigos[indice].cooldown_tiro = 0;

    if (comportamento == COMPORTAMENTO_PATRULHA) {
        inimigos[indice].patrulha_min_x = x - alcance_patrulha;
        inimigos[indice].patrulha_max_x = x + alcance_patrulha;
        if (inimigos[indice].patrulha_min_x < 0) inimigos[indice].patrulha_min_x = 0;
    } else {
        inimigos[indice].patrulha_min_x = x;
        inimigos[indice].patrulha_max_x = x;
    }
}

void inicializa_inimigos(void) {
    int i;
    for (i = 0; i < MAX_INIMIGOS; i++) inimigos[i].vivo = 0;

    cria_inimigo(0, 200,  1, COMPORTAMENTO_PATRULHA,    40);
    cria_inimigo(1, 380,  2, COMPORTAMENTO_PARADO,       0);
    cria_inimigo(2, 500,  1, COMPORTAMENTO_PERSEGUICAO,  0);
    cria_inimigo(3, 650,  2, COMPORTAMENTO_PATRULHA,    30);
    cria_inimigo(4, 780,  1, COMPORTAMENTO_PERSEGUICAO,  0);
    cria_inimigo(5, 920,  2, COMPORTAMENTO_PARADO,       0);
    cria_inimigo(6, 1050, 1, COMPORTAMENTO_PATRULHA,    45);
    cria_inimigo(7, 1200, 2, COMPORTAMENTO_PERSEGUICAO,  0);
    cria_inimigo(8, 1330, 1, COMPORTAMENTO_PATRULHA,    40);
    cria_inimigo(9, CHEFE_X, 3, COMPORTAMENTO_PATRULHA, 30);
}

void desenha_inimigo(Inimigo *e) {
    int sx, sy;
    short int cor_corpo;
    if (!e->vivo) return;

    sx = e->x - camera_x;
    sy = e->y;
    if (sx < -30 || sx > SCREEN_WIDTH) return;

    if (e->tipo == 3) {
        desenha_retangulo(sx, sy, e->w, e->h, COLOR_CHEFE);
        desenha_retangulo(sx + 4,        sy + 5, 5, 5, COLOR_ENEMY_EYE);
        desenha_retangulo(sx + e->w - 9, sy + 5, 5, 5, COLOR_ENEMY_EYE);
        {
            int barra_w = (e->vida * e->w) / CHEFE_VIDA_MAX;
            desenha_retangulo(sx, sy - 6, e->w, 4, COLOR_CHEFE_VIDA_FUNDO);
            desenha_retangulo(sx, sy - 6, barra_w, 4, COLOR_CHEFE_VIDA);
        }
        return;
    }

    cor_corpo = COLOR_ENEMY;
    if (e->tipo == 2) {
        cor_corpo = COLOR_PROJ_INIMIGO;
    } else if (e->comportamento == COMPORTAMENTO_PERSEGUICAO &&
               distancia_x(player.x, e->x) <= DISTANCIA_DETECCAO) {
        cor_corpo = COLOR_ENEMY_ALERTA;
    }

    desenha_retangulo(sx, sy, e->w, e->h, cor_corpo);
    if (e->tipo >= 1) {
        desenha_retangulo(sx + 2,        sy + 3, 3, 3, COLOR_ENEMY_EYE);
        desenha_retangulo(sx + e->w - 5, sy + 3, 3, 3, COLOR_ENEMY_EYE);
    }
}

void desenha_inimigos(void) {
    int i;
    for (i = 0; i < MAX_INIMIGOS; i++) {
        desenha_inimigo(&inimigos[i]);
    }
}

int distancia_x(int a, int b) {
    return (a > b) ? (a - b) : (b - a);
}

void move_inimigo(Inimigo *e) {
    if (e->comportamento == COMPORTAMENTO_PATRULHA) {
        if (e->vx == 0) {
            e->vx = INIMIGO_VELOCIDADE_PATRULHA;
        }
        e->x += e->vx;
        if (e->x <= e->patrulha_min_x) {
            e->x = e->patrulha_min_x;
            e->vx = INIMIGO_VELOCIDADE_PATRULHA;
            e->direcao = FACING_RIGHT;
        } else if (e->x >= e->patrulha_max_x) {
            e->x = e->patrulha_max_x;
            e->vx = -INIMIGO_VELOCIDADE_PATRULHA;
            e->direcao = FACING_LEFT;
        }
    } else if (e->comportamento == COMPORTAMENTO_PERSEGUICAO) {
        if (distancia_x(player.x, e->x) <= DISTANCIA_DETECCAO) {
            if (player.x < e->x) {
                e->x -= INIMIGO_VELOCIDADE_PERSEGUICAO;
                e->direcao = FACING_LEFT;
            } else if (player.x > e->x) {
                e->x += INIMIGO_VELOCIDADE_PERSEGUICAO;
                e->direcao = FACING_RIGHT;
            }
        }
    }
}

void atualiza_inimigos(void) {
    int i;
    for (i = 0; i < MAX_INIMIGOS; i++) {
        if (!inimigos[i].vivo) continue;

        move_inimigo(&inimigos[i]);

        if ((inimigos[i].tipo == 2 || inimigos[i].tipo == 3) &&
            distancia_x(player.x, inimigos[i].x) <= DISTANCIA_DETECCAO) {
            if (inimigos[i].cooldown_tiro <= 0) {
                dispara_projetil_inimigo(&inimigos[i]);
                inimigos[i].cooldown_tiro = (inimigos[i].tipo == 3)
                    ? COOLDOWN_TIRO_INIMIGO / 2
                    : COOLDOWN_TIRO_INIMIGO;
            }
        }
        if (inimigos[i].cooldown_tiro > 0) inimigos[i].cooldown_tiro--;

        if (colide(player.x, player.y, PLAYER_W, PLAYER_H,
                   inimigos[i].x, inimigos[i].y, inimigos[i].w, inimigos[i].h)) {
            if ((inimigos[i].tipo == 1 || inimigos[i].tipo == 2 || inimigos[i].tipo == 3)
                && player.invulneravel == 0) {
                player.vidas--;
                player.invulneravel = INVULNERABLE_FRAMES;
                if (player.x < inimigos[i].x) player.x -= 10;
                else                           player.x += 10;
            }
        }
    }
}

Projetil projeteis[MAX_PROJETEIS];

void inicializa_projeteis(void) {
    int i;
    for (i = 0; i < MAX_PROJETEIS; i++) {
        projeteis[i].ativo = 0;
    }
}

void desenha_projeteis(void) {
    int i;
    for (i = 0; i < MAX_PROJETEIS; i++) {
        if (projeteis[i].ativo) {
            int sx = projeteis[i].x - camera_x;
            short int cor = projeteis[i].do_inimigo ? COLOR_PROJ_INIMIGO : COLOR_WHIP;
            desenha_retangulo(sx, projeteis[i].y, PROJETIL_W, PROJETIL_H, cor);
        }
    }
}

void dispara_projetil_inimigo(Inimigo *e) {
    int i;
    for (i = 0; i < MAX_PROJETEIS; i++) {
        if (!projeteis[i].ativo) {
            projeteis[i].ativo = 1;
            projeteis[i].do_inimigo = 1;
            projeteis[i].dist_percorrida = 0;
            projeteis[i].y = e->y + e->h / 2;
            if (player.x < e->x) {
                projeteis[i].x  = e->x;
                projeteis[i].vx = -PROJETIL_SPEED;
            } else {
                projeteis[i].x  = e->x + e->w;
                projeteis[i].vx = PROJETIL_SPEED;
            }
            break;
        }
    }
}

void dispara_projetil(void) {
    int i;
    for (i = 0; i < MAX_PROJETEIS; i++) {
        if (!projeteis[i].ativo) {
            projeteis[i].ativo = 1;
            projeteis[i].do_inimigo = 0;
            projeteis[i].dist_percorrida = 0;
            projeteis[i].y = player.y + 8;
            if (player.direcao == FACING_RIGHT) {
                projeteis[i].x = player.x + PLAYER_W;
                projeteis[i].vx = PROJETIL_SPEED;
            } else {
                projeteis[i].x = player.x - PROJETIL_W;
                projeteis[i].vx = -PROJETIL_SPEED;
            }
            break;
        }
    }
}

void atualiza_projeteis(void) {
    int i, j;
    for (i = 0; i < MAX_PROJETEIS; i++) {
        if (!projeteis[i].ativo) continue;

        projeteis[i].x += projeteis[i].vx;
        projeteis[i].dist_percorrida += PROJETIL_SPEED;

        if (projeteis[i].dist_percorrida >= PROJETIL_ALCANCE) {
            projeteis[i].ativo = 0; continue;
        }
        if (projeteis[i].x < camera_x - 10 || projeteis[i].x > camera_x + SCREEN_WIDTH + 10) {
            projeteis[i].ativo = 0; continue;
        }

        if (projeteis[i].do_inimigo) {
            if (player.invulneravel == 0 &&
                colide(projeteis[i].x, projeteis[i].y, PROJETIL_W, PROJETIL_H,
                       player.x, player.y, PLAYER_W, PLAYER_H)) {
                player.vidas--;
                player.invulneravel = INVULNERABLE_FRAMES;
                projeteis[i].ativo = 0;
            }
        } else {
            for (j = 0; j < MAX_INIMIGOS; j++) {
                if (!inimigos[j].vivo) continue;
                if (colide(projeteis[i].x, projeteis[i].y, PROJETIL_W, PROJETIL_H,
                           inimigos[j].x, inimigos[j].y, inimigos[j].w, inimigos[j].h)) {
                    inimigos[j].vida--;
                    projeteis[i].ativo = 0;
                    if (inimigos[j].vida <= 0) {
                        inimigos[j].vivo = 0;
                    }
                    break;
                }
            }
        }
    }
}

Gap gaps[MAX_GAPS];
Lava lavas[MAX_LAVAS];
Plataforma plataformas[MAX_PLATAFORMAS];
Token tokens[MAX_TOKENS];
int tokens_coletados = 0;

void inicializa_gaps(void) {
    gaps[0].x = 310;  gaps[0].largura = 28;
    gaps[1].x = 700;  gaps[1].largura = 32;
    gaps[2].x = 1000; gaps[2].largura = 30;
    gaps[3].x = 1250; gaps[3].largura = 36;
    gaps[4].x = 1390; gaps[4].largura = 28;
}

void inicializa_lavas(void) {
    lavas[0].x = 150;  lavas[0].largura = 80;
    lavas[1].x = 450;  lavas[1].largura = 70;
    lavas[2].x = 830;  lavas[2].largura = 80;
    lavas[3].x = 1120; lavas[3].largura = 70;
}

void inicializa_plataformas(void) {
    plataformas[0].x = 148;  plataformas[0].y = 158; plataformas[0].largura = 52; plataformas[0].altura = 8;
    plataformas[1].x = 448;  plataformas[1].y = 152; plataformas[1].largura = 52; plataformas[1].altura = 8;
    plataformas[2].x = 828;  plataformas[2].y = 155; plataformas[2].largura = 54; plataformas[2].altura = 8;
    plataformas[3].x = 1118; plataformas[3].y = 150; plataformas[3].largura = 52; plataformas[3].altura = 8;
    plataformas[4].x = 580;  plataformas[4].y = 148; plataformas[4].largura = 44; plataformas[4].altura = 8;
    plataformas[5].x = 1310; plataformas[5].y = 155; plataformas[5].largura = 44; plataformas[5].altura = 8;
}

void inicializa_tokens(void) {
    int i;
    for (i = 0; i < MAX_TOKENS; i++) tokens[i].coletado = 0;

    tokens[0].x = 100;  tokens[0].y = GROUND_Y - 30;
    tokens[1].x = 290;  tokens[1].y = GROUND_Y - 30;
    tokens[2].x = 660;  tokens[2].y = GROUND_Y - 30;
    tokens[3].x = 760;  tokens[3].y = GROUND_Y - 30;
    tokens[4].x = 1060; tokens[4].y = GROUND_Y - 30;

    tokens[5].x = 165;  tokens[5].y = plataformas[0].y - 20;
    tokens[6].x = 465;  tokens[6].y = plataformas[1].y - 20;
    tokens[7].x = 592;  tokens[7].y = plataformas[4].y - 20;
    tokens[8].x = 845;  tokens[8].y = plataformas[2].y - 20;
    tokens[9].x = 1323; tokens[9].y = plataformas[5].y - 20;
}

int zona_visual(int mundo_x) {
    int largura_zona = nivel_largura / 3;
    if (mundo_x < largura_zona) return 0;
    if (mundo_x < largura_zona * 2) return 1;
    return 2;
}

short int cor_ceu_zona(int zona) {
    if (zona == 0) return COLOR_SKY_ZONA1;
    if (zona == 1) return COLOR_SKY_ZONA2;
    return COLOR_SKY_ZONA3;
}

short int cor_chao_zona(int zona) {
    if (zona == 0) return COLOR_SAND_ZONA1;
    if (zona == 1) return COLOR_SAND_ZONA2;
    return COLOR_SAND_ZONA3;
}

void desenha_ceu(void) {
    int sx;
    for (sx = 0; sx < SCREEN_WIDTH; sx++) {
        int mundo_x = sx + camera_x;
        int zona = zona_visual(mundo_x);
        int col;
        for (col = 0; col < GROUND_Y; col++) {
            desenha_pixel(sx, col, cor_ceu_zona(zona));
        }
    }
}

void desenha_cenario(void) {
    int sx;
    int i;

    desenha_ceu();

    for (sx = 0; sx < SCREEN_WIDTH; sx++) {
        int mundo_x = sx + camera_x;
        int eh_gap = 0;
        int eh_lava = 0;
        int zona = zona_visual(mundo_x);

        for (i = 0; i < MAX_GAPS; i++) {
            if (mundo_x >= gaps[i].x && mundo_x < gaps[i].x + gaps[i].largura) {
                eh_gap = 1; break;
            }
        }
        for (i = 0; i < MAX_LAVAS; i++) {
            if (mundo_x >= lavas[i].x && mundo_x < lavas[i].x + lavas[i].largura) {
                eh_lava = 1; break;
            }
        }

        if (!eh_gap) {
            int col;
            if (eh_lava) {
                for (col = 0; col < 4; col++)
                    desenha_pixel(sx, GROUND_Y + col, COLOR_LAVA_GLOW);
                for (col = 4; col < (SCREEN_HEIGHT - GROUND_Y); col++)
                    desenha_pixel(sx, GROUND_Y + col, COLOR_LAVA);
            } else {
                for (col = 0; col < (SCREEN_HEIGHT - GROUND_Y); col++)
                    desenha_pixel(sx, GROUND_Y + col, cor_chao_zona(zona));
            }
        } else {
            /* gap: pinta de preto para que o buraco apareca visivelmente */
            int col;
            for (col = 0; col < (SCREEN_HEIGHT - GROUND_Y); col++)
                desenha_pixel(sx, GROUND_Y + col, COLOR_BLACK);
        }
    }

    for (i = 0; i < (nivel_largura / 150) + 1; i++) {
        int deco_mundo_x = i * 150 + 50;
        int deco_tela_x = deco_mundo_x - camera_x;
        int zona = zona_visual(deco_mundo_x);

        if (deco_tela_x <= -30 || deco_tela_x >= SCREEN_WIDTH) continue;

        if (zona == 0) {
            desenha_retangulo(deco_tela_x, GROUND_Y - 55, 16, 55, COLOR_STONE);
            desenha_retangulo(deco_tela_x - 4, GROUND_Y - 59, 24, 6, COLOR_STONE_DARK);
        } else if (zona == 1) {
            desenha_retangulo(deco_tela_x + 4, GROUND_Y - 80, 10, 80, COLOR_STONE_DARK);
            desenha_retangulo(deco_tela_x, GROUND_Y - 88, 18, 10, COLOR_TREASURE);
        } else {
            int largura_piramide = 36;
            int altura_piramide = 30;
            int passo;
            for (passo = 0; passo < altura_piramide; passo += 3) {
                int largura_atual = largura_piramide - (passo * largura_piramide) / altura_piramide;
                desenha_retangulo(deco_tela_x + (largura_piramide - largura_atual) / 2,
                                   GROUND_Y - altura_piramide + passo,
                                   largura_atual, 3, COLOR_STONE_DARK);
            }
        }
    }
}

void desenha_plataformas(void) {
    int i;
    for (i = 0; i < MAX_PLATAFORMAS; i++) {
        int sx = plataformas[i].x - camera_x;
        if (sx + plataformas[i].largura < 0 || sx > SCREEN_WIDTH) continue;
        desenha_retangulo(sx, plataformas[i].y, plataformas[i].largura, plataformas[i].altura, COLOR_PLATFORM);
        desenha_retangulo(sx, plataformas[i].y, plataformas[i].largura, 2, COLOR_PLATFORM_TOP);
    }
}

void desenha_bandeira(void) {
    int sx = BANDEIRA_X - camera_x;
    if (sx < -10 || sx > SCREEN_WIDTH) return;
    desenha_retangulo(sx, GROUND_Y - 50, 3, 50, COLOR_FLAG_POLE);
    desenha_retangulo(sx + 3, GROUND_Y - 50, 18, 6,  COLOR_FLAG);
    desenha_retangulo(sx + 3, GROUND_Y - 44, 12, 6,  COLOR_FLAG);
    desenha_retangulo(sx + 3, GROUND_Y - 38, 6,  6,  COLOR_FLAG);
}

void desenha_tokens(void) {
    int i;
    for (i = 0; i < MAX_TOKENS; i++) {
        int sx, sy;
        if (tokens[i].coletado) continue;
        sx = tokens[i].x - camera_x;
        sy = tokens[i].y;
        if (sx < -10 || sx > SCREEN_WIDTH) continue;

        desenha_retangulo(sx + 3, sy,     2, 2, COLOR_TOKEN);
        desenha_retangulo(sx + 1, sy + 2, 6, 2, COLOR_TOKEN);
        desenha_retangulo(sx,     sy + 4, 8, 2, COLOR_TOKEN);
        desenha_retangulo(sx + 2, sy + 4, 4, 2, COLOR_TOKEN_BRILHO);
        desenha_retangulo(sx + 1, sy + 6, 6, 2, COLOR_TOKEN);
        desenha_retangulo(sx + 3, sy + 8, 2, 2, COLOR_TOKEN);
    }
}

int camera_x = 0;
int nivel_largura = 1500;
EstadoJogo estado = ESTADO_JOGANDO;

void inicializa_jogo(void) {
    inicializa_jogador();
    inicializa_inimigos();
    inicializa_projeteis();
    inicializa_gaps();
    inicializa_lavas();
    inicializa_plataformas();
    inicializa_tokens();
    tokens_coletados = 0;
    camera_x = 0;
    estado = ESTADO_JOGANDO;
}

int colide(int ax, int ay, int aw, int ah, int bx, int by, int bw, int bh) {
    if (ax + aw <= bx) return 0;
    if (ax >= bx + bw) return 0;
    if (ay + ah <= by) return 0;
    if (ay >= by + bh) return 0;
    return 1;
}

int esta_sobre_gap(int x, int largura) {
    int i;
    for (i = 0; i < MAX_GAPS; i++) {
        int centro = x + largura / 2;
        if (centro >= gaps[i].x && centro < gaps[i].x + gaps[i].largura) {
            return 1;
        }
    }
    return 0;
}

int esta_sobre_lava(int x, int largura) {
    int i;
    for (i = 0; i < MAX_LAVAS; i++) {
        int centro = x + largura / 2;
        if (centro >= lavas[i].x && centro < lavas[i].x + lavas[i].largura) {
            return 1;
        }
    }
    return 0;
}

void desenha_tela_gameover(void) {
    limpa_tela(COLOR_BLACK);
    desenha_retangulo(60, 90, 200, 60, 0x4000);
    desenha_retangulo(70, 100, 180, 40, COLOR_BLACK);
    int i;
    for (i = 0; i < 30; i++) {
        desenha_pixel(145 + i, 105 + i, 0xF800);
        desenha_pixel(145 + i, 135 - i, 0xF800);
    }
}

void desenha_tela_vitoria(void) {
    limpa_tela(COLOR_SKY);
    desenha_retangulo(60, 90, 200, 60, COLOR_TREASURE);
    desenha_retangulo(70, 100, 180, 40, COLOR_SKY);
    desenha_retangulo(150, 110, 20, 20, COLOR_TREASURE);
}

void desenha_tela_pausa(void) {
    int x, y;

    desenha_cenario();
    desenha_inimigos();
    desenha_projeteis();
    desenha_jogador();

    for (y = 0; y < SCREEN_HEIGHT; y += 2) {
        for (x = 0; x < SCREEN_WIDTH; x += 2) {
            desenha_pixel(x, y, COLOR_BLACK);
        }
    }

    desenha_retangulo(70, 70, 180, 100, COLOR_STONE_DARK);
    desenha_retangulo(78, 78, 164, 84, COLOR_BLACK);

    desenha_retangulo(140, 90, 12, 30, COLOR_WHITE);
    desenha_retangulo(160, 90, 12, 30, COLOR_WHITE);

    desenha_retangulo(90, 130, 140, 4, COLOR_TREASURE);
    desenha_retangulo(90, 142, 100, 4, COLOR_ENEMY_EYE);
}

void renderiza_cena(void) {
    if (estado == ESTADO_GAMEOVER) {
        desenha_tela_gameover();
        return;
    }
    if (estado == ESTADO_VITORIA) {
        desenha_tela_vitoria();
        return;
    }
    if (estado == ESTADO_PAUSADO) {
        desenha_tela_pausa();
        return;
    }

    desenha_cenario();
    desenha_plataformas();
    desenha_bandeira();
    desenha_tokens();
    desenha_inimigos();
    desenha_projeteis();
    desenha_jogador();
}

void atualiza_estado(void) {
    if (estado != ESTADO_JOGANDO) return;

    atualiza_jogador();
    atualiza_projeteis();
    atualiza_inimigos();
}

void processa_pausa_e_reinicio(void) {
    static int chave_anterior = 0;
    int chave_atual = chave_sw0_ligada();

    if (chave_atual && !chave_anterior) {
        if (estado == ESTADO_JOGANDO) {
            estado = ESTADO_PAUSADO;
        } else if (estado == ESTADO_PAUSADO) {
            estado = ESTADO_JOGANDO;
        }
    }
    chave_anterior = chave_atual;

    if (estado == ESTADO_PAUSADO && tecla_reiniciar) {
        inicializa_jogo();
        tecla_reiniciar = 0;
    }
}

void le_entrada(void) {
    processa_teclado();
    processa_pausa_e_reinicio();
}

void atraso_frame(void) {
    volatile int i;
    for (i = 0; i < 8000; i++) {
    }
}

int main(void) {
    inicializa_video();
    inicializa_jogo();

    while (1) {
        le_entrada();
        atualiza_estado();
        renderiza_cena();

        atualiza_pontuacao_display(tokens_coletados);
        atualiza_vidas_display(player.vidas);
        atualiza_vidas_leds(player.vidas);

        troca_buffers();

        le_entrada();

        if (estado == ESTADO_GAMEOVER || estado == ESTADO_VITORIA) {
            int espera;
            for (espera = 0; espera < 30; espera++) {
                le_entrada();
                atraso_frame();
            }
            inicializa_jogo();
        }
    }

    return 0;
}
