#include "game.h"
#include "hardware.h"
#include "player.h"
#include "level.h"

int main(void) {
    inicializa_video();
    inicializa_jogo();
    estado = ESTADO_INICIO;

    while (1) {
        le_entrada();
        atualiza_estado();
        renderiza_cena();

        atualiza_pontuacao_display(tokens_coletados);
        atualiza_vidas_display(player.vidas);
        atualiza_vidas_leds(player.vidas);

        troca_buffers();

        le_entrada();

        if (estado == ESTADO_GAMEOVER || estado == ESTADO_VITORIA) {
            int espera;
            for (espera = 0; espera < 30; espera++) {
                le_entrada();
                atraso_frame();
            }
            inicializa_jogo();
        }
    }

    return 0;
}
