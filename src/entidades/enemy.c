#include "enemy.h"
#include "hardware.h"
#include "player.h"
#include "projectiles.h"
#include "game.h"

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
