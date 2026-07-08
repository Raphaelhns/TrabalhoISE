#include "projectiles.h"
#include "hardware.h"
#include "player.h"
#include "enemy.h"
#include "game.h"

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
