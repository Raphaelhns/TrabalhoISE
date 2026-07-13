#include "hardware.h"
#include "constants.h"
#include <stdint.h>

volatile short int *pixel_buffer_back;
int tecla_esquerda = 0, tecla_direita = 0, tecla_cima = 0, tecla_atirar = 0;
int tecla_reiniciar = 0;

void desenha_pixel(int x, int y, short int cor) {
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return;
    volatile short int *one_pixel_address;
    one_pixel_address = pixel_buffer_back + (y << 9) + x;
    *one_pixel_address = cor;
}

void desenha_retangulo(int x, int y, int largura, int altura, short int cor) {
    int i, j;
    for (j = 0; j < altura; j++) {
        for (i = 0; i < largura; i++) {
            desenha_pixel(x + i, y + j, cor);
        }
    }
}

void limpa_tela(short int cor) {
    desenha_retangulo(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, cor);
}

void aguarda_swap_completo(void) {
#ifndef CPULATOR
    volatile int *status_ptr = (int *)VGA_STATUS;
    while ((*status_ptr & 0x01) != 0) {
    }
#endif
}

void inicializa_video(void) {
    volatile int *buffer_ctrl = (int *)VGA_BUFFER_CTRL;
    volatile int *backbuffer_ctrl = (int *)VGA_BACKBUFFER_CTRL;

    pixel_buffer_back = (volatile short int *)BACK_BUFFER_ADDR;
    limpa_tela(COLOR_SKY);

    *backbuffer_ctrl = BACK_BUFFER_ADDR;
    aguarda_swap_completo();
    *buffer_ctrl = 1;
    aguarda_swap_completo();

    pixel_buffer_back = (volatile short int *)FRONT_BUFFER_ADDR;
    limpa_tela(COLOR_SKY);
}

void troca_buffers(void) {
    volatile int *backbuffer_ctrl = (int *)VGA_BACKBUFFER_CTRL;
    volatile int *buffer_ctrl = (int *)VGA_BUFFER_CTRL;

    *backbuffer_ctrl = (int)(intptr_t)pixel_buffer_back;
    *buffer_ctrl = 1;
    aguarda_swap_completo();

    if (pixel_buffer_back == (volatile short int *)FRONT_BUFFER_ADDR) {
        pixel_buffer_back = (volatile short int *)BACK_BUFFER_ADDR;
    } else {
        pixel_buffer_back = (volatile short int *)FRONT_BUFFER_ADDR;
    }
}

void processa_teclado(void) {
    volatile int *PS2_ptr = (int *)PS2_BASE;
    int PS2_data;
    unsigned char byte_recebido;
    static int byte_quebra = 0;
    static int byte_extendido = 0;

    PS2_data = *(PS2_ptr);
    while ((PS2_data & 0x8000) != 0) {
        byte_recebido = PS2_data & 0xFF;
        if (byte_recebido == 0xF0) {
            byte_quebra = 1;
        } else if (byte_recebido == 0xE0) {
            byte_extendido = 1;
        } else {
            int pressionada = !byte_quebra;
            switch (byte_recebido) {
                case SCANCODE_A: tecla_esquerda = pressionada; break;
                case SCANCODE_D: tecla_direita = pressionada; break;
                case SCANCODE_W: tecla_cima = pressionada; break;
                case SCANCODE_J:
                case SCANCODE_SPACE:
                    if (byte_extendido == 0) tecla_atirar = pressionada;
                    break;
                case SCANCODE_R: tecla_reiniciar = pressionada; break;
                case SCANCODE_LEFT: if (byte_extendido) tecla_esquerda = pressionada; break;
                case SCANCODE_RIGHT: if (byte_extendido) tecla_direita = pressionada; break;
                case SCANCODE_UP: if (byte_extendido) tecla_cima = pressionada; break;
            }
            byte_quebra = 0;
            byte_extendido = 0;
        }
        PS2_data = *(PS2_ptr);
    }
}

int chave_sw0_ligada(void) {
    volatile int *sw_ptr = (int *)SW_BASE;
    return (*sw_ptr) & 0x1;
}

const unsigned char tabela_7seg[10] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

void atualiza_pontuacao_display(int pontuacao) {
    volatile int *hex30 = (int *)HEX3_HEX0_BASE;
    int digitos[4];
    int i;
    int valor = pontuacao;

    if (valor > 9999) valor = 9999;
    if (valor < 0) valor = 0;

    for (i = 0; i < 4; i++) {
        digitos[i] = valor % 10;
        valor /= 10;
    }

    unsigned int saida30 = tabela_7seg[digitos[0]]
                          | (tabela_7seg[digitos[1]] << 8)
                          | (tabela_7seg[digitos[2]] << 16)
                          | (tabela_7seg[digitos[3]] << 24);

    *hex30 = saida30;
}

void atualiza_vidas_display(int vidas) {
    volatile int *hex54 = (int *)HEX5_HEX4_BASE;
    unsigned int digito_vidas;

    if (vidas < 0) vidas = 0;
    if (vidas > 9) vidas = 9;

    digito_vidas = tabela_7seg[vidas];
    *hex54 = (digito_vidas << 8) | 0x00;
}

void atualiza_vidas_leds(int vidas) {
    volatile int *leds = (int *)LEDR_BASE;
    int padrao = 0;
    int i;

    if (vidas < 0) vidas = 0;
    if (vidas > MAX_VIDAS) vidas = MAX_VIDAS;

    for (i = 0; i < vidas; i++) {
        padrao |= (1 << i);
    }
    *leds = padrao;
}
