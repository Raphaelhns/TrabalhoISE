#ifndef GAME_H
#define GAME_H

#include "types.h"
#include "constants.h"

extern int camera_x;
extern int nivel_largura;
extern EstadoJogo estado;

void inicializa_jogo(void);
int colide(int ax, int ay, int aw, int ah, int bx, int by, int bw, int bh);
int esta_sobre_gap(int x, int largura);
int esta_sobre_lava(int x, int largura);
void desenha_tela_inicio(void);
void desenha_tela_gameover(void);
void desenha_tela_vitoria(void);
void desenha_tela_pausa(void);
void renderiza_cena(void);
void atualiza_estado(void);
void processa_pausa_e_reinicio(void);
void le_entrada(void);
void atraso_frame(void);

#endif
