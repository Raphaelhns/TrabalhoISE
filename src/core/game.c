#include "game.h"
#include "hardware.h"
#include "player.h"
#include "enemy.h"
#include "projectiles.h"
#include "level.h"
#include "tela_inicio.h"

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

void desenha_tela_inicio(void) {
    int x, y;
    for (y = 0; y < SCREEN_HEIGHT; y++) {
        for (x = 0; x < SCREEN_WIDTH; x++) {
            desenha_pixel(x, y, tela_inicio[y * SCREEN_WIDTH + x]);
        }
    }
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
    if (estado == ESTADO_INICIO) {
        desenha_tela_inicio();
        return;
    }
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
        if (estado == ESTADO_INICIO) {
            estado = ESTADO_JOGANDO;
        } else if (estado == ESTADO_JOGANDO) {
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
