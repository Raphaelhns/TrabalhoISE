#ifndef PROJECTILES_H
#define PROJECTILES_H

#include "types.h"
#include "constants.h"

extern Projetil projeteis[MAX_PROJETEIS];

void inicializa_projeteis(void);
void desenha_projeteis(void);
void atualiza_projeteis(void);
void dispara_projetil_inimigo(Inimigo *e);
void dispara_projetil(void);

#endif
