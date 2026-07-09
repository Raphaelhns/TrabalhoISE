#ifndef TYPES_H
#define TYPES_H

typedef enum { FACING_LEFT, FACING_RIGHT } Direcao;
typedef enum { ESTADO_INICIO, ESTADO_JOGANDO, ESTADO_PAUSADO, ESTADO_GAMEOVER, ESTADO_VITORIA } EstadoJogo;
typedef enum { ANIM_IDLE, ANIM_RUN, ANIM_JUMP, ANIM_SHOOT, ANIM_INTRO } AnimacaoEstado;

typedef struct {
    int x, y;
    int vx, vy;
    int no_chao;
    Direcao direcao;
    int vidas;
    int pontuacao;
    int invulneravel;
    int atacando;
    AnimacaoEstado anim_estado;
    int anim_frame;
    int anim_timer;
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
