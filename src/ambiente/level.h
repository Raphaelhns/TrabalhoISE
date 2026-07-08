#ifndef LEVEL_H
#define LEVEL_H

#include "types.h"
#include "constants.h"

extern Gap gaps[MAX_GAPS];
extern Lava lavas[MAX_LAVAS];
extern Plataforma plataformas[MAX_PLATAFORMAS];
extern Token tokens[MAX_TOKENS];
extern int tokens_coletados;

void inicializa_gaps(void);
void inicializa_lavas(void);
void inicializa_plataformas(void);
void inicializa_tokens(void);

int zona_visual(int mundo_x);
short int cor_ceu_zona(int zona);
short int cor_chao_zona(int zona);

void desenha_ceu(void);
void desenha_cenario(void);
void desenha_plataformas(void);
void desenha_bandeira(void);
void desenha_tokens(void);

#endif
