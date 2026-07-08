#ifndef HARDWARE_H
#define HARDWARE_H

#include <stdint.h>

extern volatile short int *pixel_buffer_back;
extern int tecla_esquerda, tecla_direita, tecla_cima, tecla_atirar, tecla_reiniciar;

void desenha_pixel(int x, int y, short int cor);
void desenha_retangulo(int x, int y, int largura, int altura, short int cor);
void limpa_tela(short int cor);
void aguarda_swap_completo(void);
void inicializa_video(void);
void troca_buffers(void);

void processa_teclado(void);
int chave_sw0_ligada(void);

void atualiza_pontuacao_display(int pontuacao);
void atualiza_vidas_display(int vidas);
void atualiza_vidas_leds(int vidas);

#endif
