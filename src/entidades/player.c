#include "player.h"
#include "hardware.h"
#include "projectiles.h"
#include "game.h"
#include "level.h"
#include "player_sprites.h"

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
    player.anim_estado = ANIM_INTRO;
    player.anim_frame = 0;
    player.anim_timer = 0;
}

void desenha_sprite(int sx, int sy, const unsigned short *sprite, int w, int h, int espelhar) {
    int px, py;
    for (py = 0; py < h; py++) {
        for (px = 0; px < w; px++) {
            unsigned short cor = sprite[py * w + px];
            if (cor == SPRITE_TRANSPARENT) continue; /* pula pixels transparentes */

            int draw_x = espelhar ? (sx + w - 1 - px) : (sx + px);
            int draw_y = sy + py;

            if (draw_x >= 0 && draw_x < SCREEN_WIDTH && draw_y >= 0 && draw_y < SCREEN_HEIGHT) {
                desenha_pixel(draw_x, draw_y, cor);
            }
        }
    }
}

void atualiza_animacao(void) {
    /* Se a intro esta tocando, deixa ela terminar antes de mudar de estado */
    if (player.anim_estado == ANIM_INTRO) {
        player.anim_timer++;
        if (player.anim_timer >= ANIM_SPEED) {
            player.anim_timer = 0;
            player.anim_frame++;
            if (player.anim_frame >= INTRO_FRAMES) {
                /* Intro terminou, vai para idle */
                player.anim_estado = ANIM_IDLE;
                player.anim_frame = 0;
                player.anim_timer = 0;
            }
        }
        return;
    }

    AnimacaoEstado novo_estado;

    /* Determina o estado de animacao baseado no que o jogador esta fazendo */
    if (player.atacando > 0) {
        novo_estado = ANIM_SHOOT;
    } else if (!player.no_chao) {
        novo_estado = ANIM_JUMP;
    } else if (player.vx != 0) {
        novo_estado = ANIM_RUN;
    } else {
        novo_estado = ANIM_IDLE;
    }

    /* Se mudou de estado, reseta a animacao */
    if (novo_estado != player.anim_estado) {
        player.anim_estado = novo_estado;
        player.anim_frame = 0;
        player.anim_timer = 0;
    }

    /* Idle e estatico, nao avanca frames */
    if (player.anim_estado == ANIM_IDLE) return;

    /* Avanca o timer e troca o frame quando necessario */
    player.anim_timer++;
    if (player.anim_timer >= ANIM_SPEED) {
        player.anim_timer = 0;
        player.anim_frame++;

        /* Volta ao frame 0 quando chega no ultimo */
        int max_frames;
        switch (player.anim_estado) {
            case ANIM_RUN:   max_frames = RUN_FRAMES;   break;
            case ANIM_SHOOT: max_frames = SHOOT_FRAMES; break;
            case ANIM_JUMP:  max_frames = JUMP_FRAMES;  break;
            default:         max_frames = IDLE_FRAMES;   break;
        }
        if (player.anim_frame >= max_frames) {
            player.anim_frame = 0;
        }
    }
}

void desenha_jogador(void) {
    int sx = player.x - camera_x;
    int sy = player.y;

    /* Piscar quando invulneravel */
    if (player.invulneravel > 0 && (player.invulneravel % 6) < 3) {
        return;
    }

    /* Seleciona o array de frames correto */
    const unsigned short * const *anim;
    switch (player.anim_estado) {
        case ANIM_RUN:   anim = run_anim;   break;
        case ANIM_SHOOT: anim = shoot_anim; break;
        case ANIM_JUMP:  anim = jump_anim;  break;
        case ANIM_INTRO: anim = intro_anim; break;
        default:         anim = idle_anim;  break;
    }

    int espelhar = (player.direcao == FACING_LEFT) ? 1 : 0;
    desenha_sprite(sx, sy, anim[player.anim_frame], SPRITE_W, SPRITE_H, espelhar);
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

    atualiza_animacao();
}
