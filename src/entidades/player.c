#include "player.h"
#include "hardware.h"
#include "projectiles.h"
#include "game.h"
#include "level.h"

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
