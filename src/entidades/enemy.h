#ifndef ENEMY_H
#define ENEMY_H

#include "types.h"
#include "constants.h"

extern Inimigo inimigos[MAX_INIMIGOS];

void cria_inimigo(int indice, int x, int tipo, ComportamentoInimigo comportamento, int alcance_patrulha);
void inicializa_inimigos(void);
void desenha_inimigo(Inimigo *e);
void desenha_inimigos(void);
void move_inimigo(Inimigo *e);
void atualiza_inimigos(void);
int distancia_x(int a, int b);

#endif
