#include "level.h"
#include "hardware.h"
#include "game.h"
#include "fundo_cenario1.h"
#include "fundo_cenario2.h"
#include "fundo_cenario3.h"
#include "lava_sprites.h"
#include "token_sprites.h"

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
    int sx, sy;
    for (sx = 0; sx < SCREEN_WIDTH; sx++) {
        int mundo_x = sx + camera_x;
        int zona = zona_visual(mundo_x);

        /* Seleciona o fundo correto baseado na zona */
        const unsigned short *fundo;
        int fundo_w;
        if (zona == 0) {
            fundo = fundo_cenario1;
            fundo_w = FUNDO_CENARIO1_W;
        } else if (zona == 1) {
            fundo = fundo_cenario2;
            fundo_w = FUNDO_CENARIO2_W;
        } else {
            fundo = fundo_cenario3;
            fundo_w = FUNDO_CENARIO3_W;
        }

        int img_x = mundo_x % fundo_w;
        if (img_x < 0) img_x += fundo_w;

        for (sy = 0; sy < SCREEN_HEIGHT; sy++) {
            unsigned short cor = fundo[sy * fundo_w + img_x];
            desenha_pixel(sx, sy, cor);
        }
    }
}

void desenha_cenario(void) {
    int sx;
    int i;
    static int lava_anim_timer = 0;
    static int lava_anim_frame = 0;

    lava_anim_timer++;
    if (lava_anim_timer >= ANIM_SPEED) {
        lava_anim_timer = 0;
        lava_anim_frame = (lava_anim_frame + 1) % LAVA_TOPO_FRAMES;
    }

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
                int col_tile = mundo_x % LAVA_TILE_SIZE;
                const unsigned short *topo = lava_topo_anim[lava_anim_frame];
                for (col = 0; col < LAVA_TILE_SIZE && GROUND_Y + col < SCREEN_HEIGHT; col++) {
                    unsigned short cor = topo[col * LAVA_TILE_SIZE + col_tile];
                    if (cor != LAVA_TRANSPARENT) desenha_pixel(sx, GROUND_Y + col, cor);
                }
                for (col = LAVA_TILE_SIZE; col < (SCREEN_HEIGHT - GROUND_Y); col++) {
                    int tile_y = (col - LAVA_TILE_SIZE) % LAVA_TILE_SIZE;
                    desenha_pixel(sx, GROUND_Y + col, lava_corpo[tile_y * LAVA_TILE_SIZE + col_tile]);
                }
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
    int i, tx, ty;
    for (i = 0; i < MAX_TOKENS; i++) {
        int sx, sy;
        if (tokens[i].coletado) continue;
        sx = tokens[i].x - camera_x;
        sy = tokens[i].y;
        if (sx < -TOKEN_SPRITE_W || sx > SCREEN_WIDTH) continue;

        for (ty = 0; ty < TOKEN_SPRITE_H; ty++) {
            for (tx = 0; tx < TOKEN_SPRITE_W; tx++) {
                unsigned short cor = token_sprite[ty * TOKEN_SPRITE_W + tx];
                if (cor != TOKEN_TRANSPARENT) desenha_pixel(sx + tx, sy + ty, cor);
            }
        }
    }
}
