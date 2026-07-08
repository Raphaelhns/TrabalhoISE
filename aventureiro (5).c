/* ============================================================================
 * AVENTUREIRO DAS RUINAS
 * Jogo de plataforma 2D para DE1-SoC (simulado no CPUlator)
 * 
 * Disciplina: Introducao aos Sistemas Embarcados
 * Plataforma: DE1-SoC / CPUlator (ARM Cortex-A9)
 * 
 * Controles (teclado PS/2):
 *   A / Seta Esquerda  -> mover para esquerda
 *   D / Seta Direita   -> mover para direita
 *   W / Seta Cima / Espaco -> saltar
 *   J                  -> atirar/chicotear
 * 
 * Perifericos usados:
 *   - VGA (obrigatorio) com double buffering
 *   - Displays de 7 segmentos (obrigatorio) -> pontuacao
 *   - LEDs -> vidas restantes
 *   - Teclado PS/2 -> controle do jogador
 * ============================================================================
 */

#include <stdint.h>
#include <stdlib.h>

/* ============================================================================
 * ENDERECOS DE HARDWARE (memory-mapped I/O) - DE1-SoC / CPUlator
 * ============================================================================
 */

/* --- Controlador de buffer de pixels VGA --- */
#define VGA_BUFFER_CTRL    0xFF203020   /* registrador Buffer (front buffer) */
#define VGA_BACKBUFFER_CTRL 0xFF203024  /* registrador Backbuffer */
#define VGA_RESOLUTION      0xFF203028  /* registrador de resolucao (somente leitura) */
#define VGA_STATUS          0xFF20302C  /* registrador de status (bit S = sync) */

/* Enderecos fisicos dos dois framebuffers (320x240, 16 bits/pixel)
 * IMPORTANTE: no sistema ARM (Cortex-A9) do CPUlator/DE1-SoC, a SDRAM comeca
 * em 0xC0000000 e a memoria on-chip da FPGA em 0xC8000000. O endereco
 * 0x00000000 e onde o PROPRIO PROGRAMA fica carregado (codigo/dados) -
 * escrever pixels la corrompe o programa em execucao e trava o simulador. */
#define FRONT_BUFFER_ADDR  0xC8000000   /* buffer 0 - memoria on-chip da FPGA */
#define BACK_BUFFER_ADDR   0xC0000000   /* buffer 1 - inicio da SDRAM */

/* --- Displays de 7 segmentos --- */
#define HEX3_HEX0_BASE     0xFF200020   /* HEX3-HEX0 (32 bits) */
#define HEX5_HEX4_BASE     0xFF200030   /* HEX5-HEX4 (32 bits) */

/* --- LEDs vermelhos --- */
#define LEDR_BASE          0xFF200000

/* --- Chaves e botoes (nao usados como entrada principal, mas disponiveis) --- */
#define SW_BASE            0xFF200040
#define KEY_BASE           0xFF200050

/* --- Teclado PS/2 --- */
#define PS2_BASE           0xFF200100

/* --- Resolucao da tela --- */
#define SCREEN_WIDTH   320
#define SCREEN_HEIGHT  240

/* ============================================================================
 * CONSTANTES DO JOGO
 * ============================================================================
 */

#define GROUND_Y          200   /* altura (em y) do chao */
#define GRAVITY            1    /* aceleracao da gravidade por frame */
#define JUMP_VELOCITY     -13   /* velocidade inicial do salto (negativo = sobe) */
#define MOVE_SPEED          4   /* velocidade horizontal do jogador */
#define MAX_FALL_SPEED      9   /* velocidade maxima de queda */

#define PLAYER_W           12
#define PLAYER_H           20

#define MAX_INIMIGOS       10   /* 9 normais + 1 chefe */
#define INIMIGO_W           14
#define INIMIGO_H           18
#define INIMIGO_VELOCIDADE_PATRULHA      1
#define INIMIGO_VELOCIDADE_PERSEGUICAO   2
#define DISTANCIA_DETECCAO 90
#define COOLDOWN_TIRO_INIMIGO 80  /* frames entre tiros dos inimigos atiradores */

#define CHEFE_W             28   /* chefe e maior que inimigo normal */
#define CHEFE_H             34
#define CHEFE_VIDA_MAX       5   /* precisa de 5 tiros para morrer */
#define CHEFE_X           1420   /* posicionado logo antes da bandeira */

#define MAX_PROJETEIS      10   /* 6 do jogador + 4 dos inimigos atiradores */
#define PROJETIL_SPEED       6
#define PROJETIL_W           6
#define PROJETIL_H           2
#define PROJETIL_ALCANCE    80

#define MAX_GAPS             5
#define MAX_LAVAS            4   /* trechos de lava/espinhos no chao */
#define MAX_PLATAFORMAS      6
#define MAX_VIDAS            4

#define INVULNERABLE_FRAMES 50

#define PONTUACAO_MINIMA   500
#define BANDEIRA_X        1470

#define MAX_TOKENS         10
#define TOKEN_W             8
#define TOKEN_H             8
#define TOKENS_PARA_VENCER  8   /* precisa coletar pelo menos 8 dos 10 tokens */

/* ============================================================================
 * CORES (RGB565)
 * ============================================================================
 */
#define COLOR_BLACK       0x0000
#define COLOR_WHITE       0xFFFF
#define COLOR_SKY         0x4D9F   /* azul claro */
#define COLOR_SAND        0xEDA9   /* areia/deserto */
#define COLOR_STONE       0x7BCF   /* pedra das ruinas */
#define COLOR_STONE_DARK  0x528A
#define COLOR_PLAYER_SKIN 0xFD8A
#define COLOR_PLAYER_SHIRT 0xC880  /* marrom-avermelhado (estilo aventureiro) */
#define COLOR_PLAYER_HAT  0x9B26   /* marrom chapeu */
#define COLOR_ENEMY       0x4208   /* estatua/guarda - cinza esverdeado */
#define COLOR_ENEMY_EYE   0xF800   /* vermelho */
#define COLOR_ENEMY_ALERTA 0xFB20  /* laranja - perseguidor "ativado" */
#define COLOR_GAP         0x0000   /* buraco = preto (vazio) */
#define COLOR_WHIP        0xFFE0   /* amarelo - chicote/projetil */
#define COLOR_TREASURE    0xFFE0   /* dourado */
#define COLOR_PLATFORM    0x7BCF   /* plataforma suspensa - mesma pedra do cenario */
#define COLOR_PLATFORM_TOP 0xFFE0  /* topo da plataforma - dourado para destacar */
#define COLOR_FLAG_POLE   0xC618   /* mastro da bandeira - cinza claro */
#define COLOR_FLAG        0xF800   /* bandeira - vermelho */
#define COLOR_LAVA        0xF800   /* lava - vermelho vivo */
#define COLOR_LAVA_GLOW   0xFBE0   /* brilho da lava - laranja */
#define COLOR_PROJ_INIMIGO 0xF81F  /* projetil inimigo - magenta */
#define COLOR_CHEFE       0x6000   /* chefe - vermelho escuro */
#define COLOR_CHEFE_VIDA  0x07E0   /* barra de vida do chefe - verde */
#define COLOR_CHEFE_VIDA_FUNDO 0xF800 /* fundo da barra de vida - vermelho */
#define COLOR_TOKEN       0xFFE0   /* token - dourado */
#define COLOR_TOKEN_BRILHO 0xFFFF  /* brilho interno do token - branco */

/* --- Paletas de zona (para variedade visual ao longo do nivel) --- */
#define COLOR_SKY_ZONA1    0x4D9F  /* deserto - azul claro */
#define COLOR_SAND_ZONA1   0xEDA9  /* areia clara */
#define COLOR_SKY_ZONA2    0x32B6  /* ruinas - azul mais escuro/entardecer */
#define COLOR_SAND_ZONA2   0xB965  /* terra/pedra avermelhada */
#define COLOR_SKY_ZONA3    0x1A4F  /* tumba final - quase noite */
#define COLOR_SAND_ZONA3   0x8C4A  /* pedra escura da camara funeraria */

/* ============================================================================
 * ESTRUTURAS DE DADOS
 * ============================================================================
 */

typedef enum { FACING_LEFT, FACING_RIGHT } Direcao;
typedef enum { ESTADO_JOGANDO, ESTADO_PAUSADO, ESTADO_GAMEOVER, ESTADO_VITORIA } EstadoJogo;

typedef struct {
    int x, y;           /* posicao (canto superior esquerdo) */
    int vx, vy;          /* velocidade */
    int no_chao;          /* 1 se esta tocando o chao */
    Direcao direcao;
    int vidas;
    int pontuacao;
    int invulneravel;     /* contador de frames de invencibilidade */
    int atacando;          /* contador de frames de animacao de ataque */
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
    int vida;              /* chefe tem vida > 1; inimigos normais tem vida = 1 */
    int cooldown_tiro;     /* frames restantes ate poder atirar de novo */
} Inimigo;

typedef struct {
    int x, y;
    int vx;
    int ativo;
    int dist_percorrida;
    int do_inimigo;        /* 1 = projetil disparado por inimigo (dana o jogador) */
} Projetil;

typedef struct {
    int x;
    int largura;
} Gap;

typedef struct {
    int x;                 /* posicao x do trecho de lava/espinhos no mundo */
    int largura;
} Lava;

typedef struct {
    int x, y;
    int largura;
    int altura;
} Plataforma;

typedef struct {
    int x, y;       /* posicao no mundo */
    int coletado;   /* 1 se ja foi pego pelo jogador */
} Token;

/* ============================================================================
 * VARIAVEIS GLOBAIS
 * ============================================================================
 */

volatile short int *pixel_buffer_back;

Jogador player;
Inimigo inimigos[MAX_INIMIGOS];
Projetil projeteis[MAX_PROJETEIS];
Gap gaps[MAX_GAPS];
Lava lavas[MAX_LAVAS];
Plataforma plataformas[MAX_PLATAFORMAS];
Token tokens[MAX_TOKENS];
int tokens_coletados = 0;

int camera_x = 0;
int nivel_largura = 1500;
EstadoJogo estado = ESTADO_JOGANDO;
int tecla_esquerda = 0, tecla_direita = 0, tecla_cima = 0, tecla_atirar = 0;
int tecla_reiniciar = 0;

/* ============================================================================
 * FUNCOES DE VIDEO (VGA + Double Buffering)
 * ============================================================================
 */

/* Escreve um pixel no back buffer atual */
void desenha_pixel(int x, int y, short int cor) {
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return;
    volatile short int *one_pixel_address;
    one_pixel_address = pixel_buffer_back + (y << 9) + x;  /* y*512 + x (stride real e 512) */
    *one_pixel_address = cor;
}

/* Desenha um retangulo preenchido */
void desenha_retangulo(int x, int y, int largura, int altura, short int cor) {
    int i, j;
    for (j = 0; j < altura; j++) {
        for (i = 0; i < largura; i++) {
            desenha_pixel(x + i, y + j, cor);
        }
    }
}

/* Limpa a tela (back buffer) com uma cor de fundo */
void limpa_tela(short int cor) {
    desenha_retangulo(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, cor);
}

/* Aguarda o swap anterior terminar (poll do bit S do Status register).
 * Deve ser chamada ANTES de pedir um novo swap, e tambem logo apos pedi-lo,
 * para garantir que nenhum desenho aconteca durante a troca em andamento. */
void aguarda_swap_completo(void) {
    volatile int *status_ptr = (int *)VGA_STATUS;
    while ((*status_ptr & 0x01) != 0) {
        /* busy-wait: bit S=1 enquanto o swap esta em andamento */
    }
}

/* Inicializa o sistema de double buffering.
 * Sequencia correta, conforme documentacao do pixel buffer controller:
 *   1. Escreve o endereco do buffer de DESENHO no registrador Backbuffer (0x...24)
 *   2. Escreve 1 no registrador Buffer (0x...20) para pedir a troca
 *   3. Espera o bit S do Status voltar a 0 (troca concluida)
 * O registrador Buffer NUNCA recebe um endereco de memoria diretamente -
 * ele so aceita o comando "1" para trocar. Escrever um endereco bruto nele
 * (erro da versao anterior) deixa o controlador em estado invalido. */
void inicializa_video(void) {
    volatile int *buffer_ctrl = (int *)VGA_BUFFER_CTRL;
    volatile int *backbuffer_ctrl = (int *)VGA_BACKBUFFER_CTRL;

    /* desenha a primeira tela (preta) no buffer 1 (back) */
    pixel_buffer_back = (volatile short int *)BACK_BUFFER_ADDR;
    limpa_tela(COLOR_SKY);

    /* aponta o Backbuffer para o buffer que acabamos de desenhar */
    *backbuffer_ctrl = BACK_BUFFER_ADDR;

    /* pede a troca: o que estava no Backbuffer passa a ser exibido (front) */
    aguarda_swap_completo();   /* garante que nao ha swap pendente antes de pedir outro */
    *buffer_ctrl = 1;
    aguarda_swap_completo();   /* espera esta troca terminar */

    /* agora o buffer que acabamos de desenhar (BACK_BUFFER_ADDR) esta sendo exibido.
     * passamos a desenhar no OUTRO buffer (FRONT_BUFFER_ADDR) para o proximo frame. */
    pixel_buffer_back = (volatile short int *)FRONT_BUFFER_ADDR;
    limpa_tela(COLOR_SKY);
}

/* Troca os buffers: o que acabamos de desenhar em pixel_buffer_back vira visivel,
 * e passamos a desenhar no outro buffer no proximo frame. */
void troca_buffers(void) {
    volatile int *backbuffer_ctrl = (int *)VGA_BACKBUFFER_CTRL;
    volatile int *buffer_ctrl = (int *)VGA_BUFFER_CTRL;

    /* aponta o Backbuffer para o buffer que acabamos de desenhar */
    *backbuffer_ctrl = (int)(intptr_t)pixel_buffer_back;

    /* pede a troca e espera ela ser concluida ANTES de desenhar de novo */
    *buffer_ctrl = 1;
    aguarda_swap_completo();

    /* o buffer que acabamos de desenhar agora esta sendo exibido (front).
     * passamos a desenhar no outro buffer no proximo frame. */
    if (pixel_buffer_back == (volatile short int *)FRONT_BUFFER_ADDR) {
        pixel_buffer_back = (volatile short int *)BACK_BUFFER_ADDR;
    } else {
        pixel_buffer_back = (volatile short int *)FRONT_BUFFER_ADDR;
    }
}

/* ============================================================================
 * FUNCOES DE TECLADO PS/2
 * ============================================================================
 */

/* Codigos "make" do scan code set 2 (PS/2) para as teclas usadas.
 * Quando uma tecla e SOLTA, o PS/2 envia 0xF0 seguido do mesmo codigo.
 */
#define SCANCODE_A      0x1C
#define SCANCODE_D      0x23
#define SCANCODE_W      0x1D
#define SCANCODE_J      0x3B
#define SCANCODE_SPACE  0x29
#define SCANCODE_R      0x2D   /* reinicia o jogo manualmente (usado na tela de pausa) */
#define SCANCODE_LEFT   0x6B   /* seta esquerda (extended, precedido de 0xE0) */
#define SCANCODE_RIGHT  0x74   /* seta direita  (extended, precedido de 0xE0) */
#define SCANCODE_UP     0x75   /* seta cima     (extended, precedido de 0xE0) */

/* Le e processa bytes disponiveis do PS/2, atualizando o estado das teclas.
 * Deve ser chamada a cada frame do jogo (polling).
 */
void processa_teclado(void) {
    volatile int *PS2_ptr = (int *)PS2_BASE;
    int PS2_data;
    unsigned char byte_recebido;
    static int byte_quebra = 0;     /* 1 se o ultimo byte foi 0xF0 (break code) */
    static int byte_extendido = 0;  /* 1 se o ultimo byte foi 0xE0 (extended key) */

    PS2_data = *(PS2_ptr);

    /* bit 15 (RVALID) indica se ha dado novo disponivel */
    while ((PS2_data & 0x8000) != 0) {
        byte_recebido = PS2_data & 0xFF;

        if (byte_recebido == 0xF0) {
            byte_quebra = 1;
        } else if (byte_recebido == 0xE0) {
            byte_extendido = 1;
        } else {
            int pressionada = !byte_quebra;

            switch (byte_recebido) {
                case SCANCODE_A:
                    tecla_esquerda = pressionada;
                    break;
                case SCANCODE_D:
                    tecla_direita = pressionada;
                    break;
                case SCANCODE_W:
                    tecla_cima = pressionada;
                    break;
                case SCANCODE_J:
                case SCANCODE_SPACE:
                    if (byte_extendido == 0) {
                        tecla_atirar = pressionada;
                    }
                    break;
                case SCANCODE_R:
                    tecla_reiniciar = pressionada;
                    break;
                case SCANCODE_LEFT:
                    if (byte_extendido) tecla_esquerda = pressionada;
                    break;
                case SCANCODE_RIGHT:
                    if (byte_extendido) tecla_direita = pressionada;
                    break;
                case SCANCODE_UP:
                    if (byte_extendido) tecla_cima = pressionada;
                    break;
                default:
                    break;
            }

            byte_quebra = 0;
            byte_extendido = 0;
        }

        PS2_data = *(PS2_ptr);
    }
}

/* Le o estado da chave SW0 (usada para pausar/despausar o jogo).
 * Retorna 1 se a chave estiver ligada (para cima), 0 caso contrario. */
int chave_sw0_ligada(void) {
    volatile int *sw_ptr = (int *)SW_BASE;
    return (*sw_ptr) & 0x1;
}

/* ============================================================================
 * FUNCOES DE DISPLAY 7 SEGMENTOS E LEDS
 * ============================================================================
 */

/* Tabela de codigos para os displays de 7 segmentos (0-9), catodo comum */
const unsigned char tabela_7seg[10] = {
    0x3F, /* 0 */
    0x06, /* 1 */
    0x5B, /* 2 */
    0x4F, /* 3 */
    0x66, /* 4 */
    0x6D, /* 5 */
    0x7D, /* 6 */
    0x07, /* 7 */
    0x7F, /* 8 */
    0x6F  /* 9 */
};

/* Mostra a pontuacao (0-999999) nos seis displays de 7 segmentos */
/* Mostra a pontuacao (0-9999) nos quatro displays HEX3-HEX0.
 * HEX5-HEX4 ficam reservados para o display de vidas (ver atualiza_vidas_display). */
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

/* Mostra o numero de vidas restantes isoladamente no display HEX5.
 * HEX4 e mantido apagado (0x00) para separar visualmente de HEX3-HEX0 (pontuacao).
 * No registrador HEX5_HEX4 (0xFF200030): bits 6-0 = HEX4, bits 14-8 = HEX5. */
void atualiza_vidas_display(int vidas) {
    volatile int *hex54 = (int *)HEX5_HEX4_BASE;
    unsigned int digito_vidas;

    if (vidas < 0) vidas = 0;
    if (vidas > 9) vidas = 9;  /* protecao caso MAX_VIDAS aumente no futuro */

    digito_vidas = tabela_7seg[vidas];
    *hex54 = (digito_vidas << 8) | 0x00;   /* HEX5 = digito de vidas, HEX4 = apagado */
}

/* Mostra o numero de vidas nos LEDs vermelhos (um LED aceso por vida) */
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

/* ============================================================================
 * INICIALIZACAO DO JOGO
 * ============================================================================
 */

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

void cria_inimigo(int indice, int x, int tipo, ComportamentoInimigo comportamento, int alcance_patrulha) {
    inimigos[indice].x = x;
    inimigos[indice].y = GROUND_Y - INIMIGO_H;
    inimigos[indice].w = (tipo == 3) ? CHEFE_W : INIMIGO_W;
    inimigos[indice].h = (tipo == 3) ? CHEFE_H : INIMIGO_H;
    inimigos[indice].y = GROUND_Y - inimigos[indice].h;
    inimigos[indice].vivo = 1;
    inimigos[indice].tipo = tipo;
    inimigos[indice].comportamento = comportamento;
    inimigos[indice].direcao = FACING_LEFT;
    inimigos[indice].vx = 0;
    inimigos[indice].vida = (tipo == 3) ? CHEFE_VIDA_MAX : 1;
    inimigos[indice].cooldown_tiro = 0;

    if (comportamento == COMPORTAMENTO_PATRULHA) {
        inimigos[indice].patrulha_min_x = x - alcance_patrulha;
        inimigos[indice].patrulha_max_x = x + alcance_patrulha;
        if (inimigos[indice].patrulha_min_x < 0) inimigos[indice].patrulha_min_x = 0;
    } else {
        inimigos[indice].patrulha_min_x = x;
        inimigos[indice].patrulha_max_x = x;
    }
}

void inicializa_inimigos(void) {
    int i;
    for (i = 0; i < MAX_INIMIGOS; i++) inimigos[i].vivo = 0;

    /* tipo 1 = nocivo, tipo 2 = atirador, tipo 3 = chefe */
    cria_inimigo(0, 200,  1, COMPORTAMENTO_PATRULHA,    40);
    cria_inimigo(1, 380,  2, COMPORTAMENTO_PARADO,       0);  /* atirador parado */
    cria_inimigo(2, 500,  1, COMPORTAMENTO_PERSEGUICAO,  0);
    cria_inimigo(3, 650,  2, COMPORTAMENTO_PATRULHA,    30);  /* atirador que patrulha */
    cria_inimigo(4, 780,  1, COMPORTAMENTO_PERSEGUICAO,  0);
    cria_inimigo(5, 920,  2, COMPORTAMENTO_PARADO,       0);  /* atirador parado */
    cria_inimigo(6, 1050, 1, COMPORTAMENTO_PATRULHA,    45);
    cria_inimigo(7, 1200, 2, COMPORTAMENTO_PERSEGUICAO,  0);  /* atirador perseguidor */
    cria_inimigo(8, 1330, 1, COMPORTAMENTO_PATRULHA,    40);
    cria_inimigo(9, CHEFE_X, 3, COMPORTAMENTO_PATRULHA, 30);  /* chefe final */
}


void inicializa_projeteis(void) {
    int i;
    for (i = 0; i < MAX_PROJETEIS; i++) {
        projeteis[i].ativo = 0;
    }
}

void inicializa_gaps(void) {
    gaps[0].x = 310;  gaps[0].largura = 28;
    gaps[1].x = 700;  gaps[1].largura = 32;
    gaps[2].x = 1000; gaps[2].largura = 30;
    gaps[3].x = 1250; gaps[3].largura = 36;
    gaps[4].x = 1390; gaps[4].largura = 28;
}

void inicializa_lavas(void) {
    /* trechos de lava: bloqueiam o chao, forcando o jogador a usar as plataformas
       que estao posicionadas exatamente em cima deles */
    lavas[0].x = 150;  lavas[0].largura = 80;   /* plataforma[0] fica em cima */
    lavas[1].x = 450;  lavas[1].largura = 70;   /* plataforma[1] fica em cima */
    lavas[2].x = 830;  lavas[2].largura = 80;   /* plataforma[2] fica em cima */
    lavas[3].x = 1120; lavas[3].largura = 70;   /* plataforma[3] fica em cima */
}

void inicializa_plataformas(void) {
    /* plataformas posicionadas EXATAMENTE sobre as lavas:
       o jogador nao consegue cruzar pelo chao, precisa pular na plataforma */
    plataformas[0].x = 148;  plataformas[0].y = 158; plataformas[0].largura = 52; plataformas[0].altura = 8;
    plataformas[1].x = 448;  plataformas[1].y = 152; plataformas[1].largura = 52; plataformas[1].altura = 8;
    plataformas[2].x = 828;  plataformas[2].y = 155; plataformas[2].largura = 54; plataformas[2].altura = 8;
    plataformas[3].x = 1118; plataformas[3].y = 150; plataformas[3].largura = 52; plataformas[3].altura = 8;
    /* plataformas extras sem lava: caminhos alternativos mais altos */
    plataformas[4].x = 580;  plataformas[4].y = 148; plataformas[4].largura = 44; plataformas[4].altura = 8;
    plataformas[5].x = 1310; plataformas[5].y = 155; plataformas[5].largura = 44; plataformas[5].altura = 8;
}

void inicializa_tokens(void) {
    int i;
    for (i = 0; i < MAX_TOKENS; i++) tokens[i].coletado = 0;

    /* tokens no chao: y posicionado bem acima do chao (fora da regiao pintada
       pelo ceu que vai ate GROUND_Y-1) e acima do jogador para ser visivel */
    tokens[0].x = 100;  tokens[0].y = GROUND_Y - 30;
    tokens[1].x = 290;  tokens[1].y = GROUND_Y - 30;
    tokens[2].x = 660;  tokens[2].y = GROUND_Y - 30;
    tokens[3].x = 760;  tokens[3].y = GROUND_Y - 30;
    tokens[4].x = 1060; tokens[4].y = GROUND_Y - 30;

    /* tokens suspensos: acima das plataformas, so acessiveis subindo nelas */
    tokens[5].x = 165;  tokens[5].y = plataformas[0].y - 20;
    tokens[6].x = 465;  tokens[6].y = plataformas[1].y - 20;
    tokens[7].x = 592;  tokens[7].y = plataformas[4].y - 20;
    tokens[8].x = 845;  tokens[8].y = plataformas[2].y - 20;
    tokens[9].x = 1323; tokens[9].y = plataformas[5].y - 20;
}

void inicializa_jogo(void) {
    inicializa_jogador();
    inicializa_inimigos();
    inicializa_projeteis();
    inicializa_gaps();
    inicializa_lavas();
    inicializa_plataformas();  /* DEVE vir antes de inicializa_tokens: tokens suspensos */
    inicializa_tokens();       /* usam plataformas[i].y para calcular sua posicao y    */
    tokens_coletados = 0;
    camera_x = 0;
    estado = ESTADO_JOGANDO;
}



/* ============================================================================
 * FUNCOES DE DESENHO (SPRITES EM PIXEL ART)
 * ============================================================================
 */

/* Prototipo: definida mais abaixo (secao de logica), mas usada aqui por desenha_inimigo() */
int distancia_x(int a, int b);

/* Desenha o cenario: ceu, chao de areia, ruinas (colunas de pedra) e gaps (buracos) */
/* Retorna o indice da zona visual (0, 1 ou 2) de acordo com a posicao x no mundo.
 * O nivel e dividido em 3 trechos iguais, cada um com sua propria paleta. */
int zona_visual(int mundo_x) {
    int largura_zona = nivel_largura / 3;
    if (mundo_x < largura_zona) return 0;
    if (mundo_x < largura_zona * 2) return 1;
    return 2;
}

/* Retorna a cor do ceu para uma zona */
short int cor_ceu_zona(int zona) {
    if (zona == 0) return COLOR_SKY_ZONA1;
    if (zona == 1) return COLOR_SKY_ZONA2;
    return COLOR_SKY_ZONA3;
}

/* Retorna a cor da areia/chao para uma zona */
short int cor_chao_zona(int zona) {
    if (zona == 0) return COLOR_SAND_ZONA1;
    if (zona == 1) return COLOR_SAND_ZONA2;
    return COLOR_SAND_ZONA3;
}

/* Desenha o ceu, variando a cor de fundo conforme a zona visivel na tela.
 * Como a camera pode mostrar duas zonas ao mesmo tempo (na transicao),
 * desenhamos coluna por coluna com a cor da zona correspondente aquele x. */
void desenha_ceu(void) {
    int sx;
    for (sx = 0; sx < SCREEN_WIDTH; sx++) {
        int mundo_x = sx + camera_x;
        int zona = zona_visual(mundo_x);
        int col;
        for (col = 0; col < GROUND_Y; col++) {
            desenha_pixel(sx, col, cor_ceu_zona(zona));
        }
    }
}

/* Desenha o cenario: ceu, chao (com cor variando por zona), ruinas decorativas
 * (formato varia por zona: colunas, obeliscos, pequenas piramides) e gaps */
void desenha_cenario(void) {
    int sx; /* posicao na tela = posicao no mundo - camera_x */
    int i;

    desenha_ceu();

    /* chao, com cor de acordo com a zona, e buracos (gaps) onde nao ha chao */
    for (sx = 0; sx < SCREEN_WIDTH; sx++) {
        int mundo_x = sx + camera_x;
        int eh_gap = 0;
        int eh_lava = 0;
        int zona = zona_visual(mundo_x);

        for (i = 0; i < MAX_GAPS; i++) {
            if (mundo_x >= gaps[i].x && mundo_x < gaps[i].x + gaps[i].largura) {
                eh_gap = 1; break;
            }
        }
        for (i = 0; i < MAX_LAVAS; i++) {
            if (mundo_x >= lavas[i].x && mundo_x < lavas[i].x + lavas[i].largura) {
                eh_lava = 1; break;
            }
        }

        if (!eh_gap) {
            int col;
            if (eh_lava) {
                /* topo da lava: 4px laranja brilhante + resto vermelho */
                for (col = 0; col < 4; col++)
                    desenha_pixel(sx, GROUND_Y + col, COLOR_LAVA_GLOW);
                for (col = 4; col < (SCREEN_HEIGHT - GROUND_Y); col++)
                    desenha_pixel(sx, GROUND_Y + col, COLOR_LAVA);
            } else {
                for (col = 0; col < (SCREEN_HEIGHT - GROUND_Y); col++)
                    desenha_pixel(sx, GROUND_Y + col, cor_chao_zona(zona));
            }
        }
    }

    /* decoracoes de ruinas, alternando formato conforme a zona, a cada 150px no mundo */
    for (i = 0; i < (nivel_largura / 150) + 1; i++) {
        int deco_mundo_x = i * 150 + 50;
        int deco_tela_x = deco_mundo_x - camera_x;
        int zona = zona_visual(deco_mundo_x);

        if (deco_tela_x <= -30 || deco_tela_x >= SCREEN_WIDTH) continue;

        if (zona == 0) {
            /* zona 1 (deserto): colunas simples de pedra clara */
            desenha_retangulo(deco_tela_x, GROUND_Y - 55, 16, 55, COLOR_STONE);
            desenha_retangulo(deco_tela_x - 4, GROUND_Y - 59, 24, 6, COLOR_STONE_DARK);
        } else if (zona == 1) {
            /* zona 2 (ruinas): obeliscos altos e finos */
            desenha_retangulo(deco_tela_x + 4, GROUND_Y - 80, 10, 80, COLOR_STONE_DARK);
            desenha_retangulo(deco_tela_x, GROUND_Y - 88, 18, 10, COLOR_TREASURE);
        } else {
            /* zona 3 (tumba final): pequenas piramides */
            int largura_piramide = 36;
            int altura_piramide = 30;
            int passo;
            for (passo = 0; passo < altura_piramide; passo += 3) {
                int largura_atual = largura_piramide - (passo * largura_piramide) / altura_piramide;
                desenha_retangulo(deco_tela_x + (largura_piramide - largura_atual) / 2,
                                   GROUND_Y - altura_piramide + passo,
                                   largura_atual, 3, COLOR_STONE_DARK);
            }
        }
    }
}

/* Desenha todas as plataformas suspensas visiveis na tela */
void desenha_plataformas(void) {
    int i;
    for (i = 0; i < MAX_PLATAFORMAS; i++) {
        int sx = plataformas[i].x - camera_x;
        if (sx + plataformas[i].largura < 0 || sx > SCREEN_WIDTH) continue;
        /* corpo da plataforma */
        desenha_retangulo(sx, plataformas[i].y, plataformas[i].largura, plataformas[i].altura, COLOR_PLATFORM);
        /* borda superior dourada para destacar visualmente */
        desenha_retangulo(sx, plataformas[i].y, plataformas[i].largura, 2, COLOR_PLATFORM_TOP);
    }
}

/* Desenha a bandeira de vitoria no fim do nivel */
void desenha_bandeira(void) {
    int sx = BANDEIRA_X - camera_x;
    if (sx < -10 || sx > SCREEN_WIDTH) return;
    desenha_retangulo(sx, GROUND_Y - 50, 3, 50, COLOR_FLAG_POLE);
    desenha_retangulo(sx + 3, GROUND_Y - 50, 18, 6,  COLOR_FLAG);
    desenha_retangulo(sx + 3, GROUND_Y - 44, 12, 6,  COLOR_FLAG);
    desenha_retangulo(sx + 3, GROUND_Y - 38, 6,  6,  COLOR_FLAG);
}

/* Desenha todos os tokens nao coletados como losangos dourados */
void desenha_tokens(void) {
    int i;
    for (i = 0; i < MAX_TOKENS; i++) {
        int sx, sy;
        if (tokens[i].coletado) continue;
        sx = tokens[i].x - camera_x;
        sy = tokens[i].y;
        if (sx < -10 || sx > SCREEN_WIDTH) continue;

        /* losango: linha do meio larga, topo e base estreitos */
        desenha_retangulo(sx + 3, sy,     2, 2, COLOR_TOKEN);          /* topo */
        desenha_retangulo(sx + 1, sy + 2, 6, 2, COLOR_TOKEN);          /* meio-topo */
        desenha_retangulo(sx,     sy + 4, 8, 2, COLOR_TOKEN);          /* centro */
        desenha_retangulo(sx + 2, sy + 4, 4, 2, COLOR_TOKEN_BRILHO);   /* brilho centro */
        desenha_retangulo(sx + 1, sy + 6, 6, 2, COLOR_TOKEN);          /* meio-baixo */
        desenha_retangulo(sx + 3, sy + 8, 2, 2, COLOR_TOKEN);          /* base */
    }
}

/* Desenha o jogador (arqueologo) na tela, considerando a camera */
void desenha_jogador(void) {
    int sx = player.x - camera_x;
    int sy = player.y;

    /* pisca durante invencibilidade (nao desenha em frames pares) */
    if (player.invulneravel > 0 && (player.invulneravel % 6) < 3) {
        return;
    }

    /* corpo (camisa) */
    desenha_retangulo(sx, sy + 6, PLAYER_W, PLAYER_H - 10, COLOR_PLAYER_SHIRT);

    /* cabeca */
    desenha_retangulo(sx + 2, sy, PLAYER_W - 4, 6, COLOR_PLAYER_SKIN);

    /* chapeu (estilo Indiana Jones) */
    desenha_retangulo(sx, sy - 3, PLAYER_W, 3, COLOR_PLAYER_HAT);
    desenha_retangulo(sx - 1, sy, PLAYER_W + 2, 2, COLOR_PLAYER_HAT);

    /* pernas (mudam de posicao para simular caminhada, baseado em vx) */
    if (player.no_chao && player.vx != 0) {
        desenha_retangulo(sx + 1, sy + PLAYER_H - 4, 4, 4, COLOR_STONE_DARK);
        desenha_retangulo(sx + PLAYER_W - 5, sy + PLAYER_H - 4, 4, 4, COLOR_STONE_DARK);
    } else {
        desenha_retangulo(sx + 1, sy + PLAYER_H - 4, 4, 4, COLOR_STONE_DARK);
        desenha_retangulo(sx + PLAYER_W - 5, sy + PLAYER_H - 4, 4, 4, COLOR_STONE_DARK);
    }

    /* chicote, desenhado quando atacando */
    if (player.atacando > 0) {
        int whip_x = (player.direcao == FACING_RIGHT) ? sx + PLAYER_W : sx - 10;
        desenha_retangulo(whip_x, sy + 8, 10, 2, COLOR_WHIP);
    }
}

/* Desenha um inimigo com visual diferente por tipo */
void desenha_inimigo(Inimigo *e) {
    int sx, sy;
    short int cor_corpo;
    if (!e->vivo) return;

    sx = e->x - camera_x;
    sy = e->y;
    if (sx < -30 || sx > SCREEN_WIDTH) return;

    if (e->tipo == 3) {
        /* --- CHEFE: grande, vermelho escuro, com barra de vida acima --- */
        desenha_retangulo(sx, sy, e->w, e->h, COLOR_CHEFE);
        /* olhos brilhantes */
        desenha_retangulo(sx + 4,        sy + 5, 5, 5, COLOR_ENEMY_EYE);
        desenha_retangulo(sx + e->w - 9, sy + 5, 5, 5, COLOR_ENEMY_EYE);
        /* barra de vida: 28px de largura acima do chefe */
        {
            int barra_w = (e->vida * e->w) / CHEFE_VIDA_MAX;
            desenha_retangulo(sx, sy - 6, e->w, 4, COLOR_CHEFE_VIDA_FUNDO);
            desenha_retangulo(sx, sy - 6, barra_w, 4, COLOR_CHEFE_VIDA);
        }
        return;
    }

    /* inimigos normais */
    cor_corpo = COLOR_ENEMY;
    if (e->tipo == 2) {
        /* atirador: magenta para diferenciar visualmente */
        cor_corpo = COLOR_PROJ_INIMIGO;
    } else if (e->comportamento == COMPORTAMENTO_PERSEGUICAO &&
               distancia_x(player.x, e->x) <= DISTANCIA_DETECCAO) {
        cor_corpo = COLOR_ENEMY_ALERTA;
    }

    desenha_retangulo(sx, sy, e->w, e->h, cor_corpo);
    if (e->tipo >= 1) {
        desenha_retangulo(sx + 2,        sy + 3, 3, 3, COLOR_ENEMY_EYE);
        desenha_retangulo(sx + e->w - 5, sy + 3, 3, 3, COLOR_ENEMY_EYE);
    }
}

/* Desenha todos os inimigos vivos */
void desenha_inimigos(void) {
    int i;
    for (i = 0; i < MAX_INIMIGOS; i++) {
        desenha_inimigo(&inimigos[i]);
    }
}

/* Desenha todos os projeteis ativos, com cor diferente por origem */
void desenha_projeteis(void) {
    int i;
    for (i = 0; i < MAX_PROJETEIS; i++) {
        if (projeteis[i].ativo) {
            int sx = projeteis[i].x - camera_x;
            short int cor = projeteis[i].do_inimigo ? COLOR_PROJ_INIMIGO : COLOR_WHIP;
            desenha_retangulo(sx, projeteis[i].y, PROJETIL_W, PROJETIL_H, cor);
        }
    }
}

/* ============================================================================
 * FUNCOES DE COLISAO
 * ============================================================================
 */

/* Verifica colisao entre dois retangulos (AABB - Axis-Aligned Bounding Box) */
int colide(int ax, int ay, int aw, int ah, int bx, int by, int bw, int bh) {
    if (ax + aw <= bx) return 0;
    if (ax >= bx + bw) return 0;
    if (ay + ah <= by) return 0;
    if (ay >= by + bh) return 0;
    return 1;
}

/* Verifica se uma posicao x do jogador esta sobre um gap (buraco) */
int esta_sobre_gap(int x, int largura) {
    int i;
    for (i = 0; i < MAX_GAPS; i++) {
        int centro = x + largura / 2;
        if (centro >= gaps[i].x && centro < gaps[i].x + gaps[i].largura) {
            return 1;
        }
    }
    return 0;
}

/* Verifica se o jogador esta pisando em lava (causa dano instantaneo) */
int esta_sobre_lava(int x, int largura) {
    int i;
    for (i = 0; i < MAX_LAVAS; i++) {
        int centro = x + largura / 2;
        if (centro >= lavas[i].x && centro < lavas[i].x + lavas[i].largura) {
            return 1;
        }
    }
    return 0;
}

/* ============================================================================
 * ATUALIZACAO DE ESTADO (LOGICA DO JOGO)
 * ============================================================================
 */

/* Dispara um projetil a partir de um inimigo atirador na direcao do jogador */
void dispara_projetil_inimigo(Inimigo *e) {
    int i;
    for (i = 0; i < MAX_PROJETEIS; i++) {
        if (!projeteis[i].ativo) {
            projeteis[i].ativo = 1;
            projeteis[i].do_inimigo = 1;
            projeteis[i].dist_percorrida = 0;
            projeteis[i].y = e->y + e->h / 2;
            if (player.x < e->x) {
                projeteis[i].x  = e->x;
                projeteis[i].vx = -PROJETIL_SPEED;
            } else {
                projeteis[i].x  = e->x + e->w;
                projeteis[i].vx = PROJETIL_SPEED;
            }
            break;
        }
    }
}

/* Cria um novo projetil na posicao e direcao do jogador (se houver slot livre) */
void dispara_projetil(void) {
    int i;
    for (i = 0; i < MAX_PROJETEIS; i++) {
        if (!projeteis[i].ativo) {
            projeteis[i].ativo = 1;
            projeteis[i].do_inimigo = 0;
            projeteis[i].dist_percorrida = 0;
            projeteis[i].y = player.y + 8;
            if (player.direcao == FACING_RIGHT) {
                projeteis[i].x = player.x + PLAYER_W;
                projeteis[i].vx = PROJETIL_SPEED;
            } else {
                projeteis[i].x = player.x - PROJETIL_W;
                projeteis[i].vx = -PROJETIL_SPEED;
            }
            break;
        }
    }
}

/* Atualiza fisica e input do jogador */
void atualiza_jogador(void) {
    /* --- horizontal: leitura de teclado --- */
    player.vx = 0;
    if (tecla_esquerda) {
        player.vx = -MOVE_SPEED;
        player.direcao = FACING_LEFT;
    }
    if (tecla_direita) {
        player.vx = MOVE_SPEED;
        player.direcao = FACING_RIGHT;
    }

    /* --- salto --- */
    if (tecla_cima && player.no_chao) {
        player.vy = JUMP_VELOCITY;
        player.no_chao = 0;
    }

    /* --- ataque (chicote) --- */
    if (tecla_atirar && player.atacando == 0) {
        player.atacando = 10; /* frames de duracao da animacao/hitbox de ataque */
        dispara_projetil();
    }
    if (player.atacando > 0) {
        player.atacando--;
    }

    /* --- gravidade: so acumula quando no ar --- */
    if (!player.no_chao) {
        player.vy += GRAVITY;
        if (player.vy > MAX_FALL_SPEED) player.vy = MAX_FALL_SPEED;
    }

    /* --- aplica movimento vertical --- */
    player.x += player.vx;
    player.y += player.vy;

    /* limites horizontais */
    if (player.x < 0) player.x = 0;
    if (player.x > nivel_largura - PLAYER_W) player.x = nivel_largura - PLAYER_W;

    /* reinicia no_chao; sera setado a 1 pela colisao abaixo */
    player.no_chao = 0;

    /* --- colisao com plataformas suspensas ---
       so detecta quando o jogador esta CAINDO (vy >= 0) e cruza o topo.
       Usar vy >= 0 evita "grudar" na plataforma ao pular por baixo dela. */
    {
        int i;
        for (i = 0; i < MAX_PLATAFORMAS; i++) {
            int px = plataformas[i].x;
            int py = plataformas[i].y;
            int pw = plataformas[i].largura;
            /* sobreposicao horizontal */
            if (player.x + PLAYER_W <= px || player.x >= px + pw) continue;
            /* pes dentro ou abaixo do topo da plataforma, caindo */
            if (player.vy >= 0 && player.y + PLAYER_H >= py && player.y < py) {
                player.y  = py - PLAYER_H;
                player.vy = 0;
                player.no_chao = 1;
            }
        }
    }

    /* --- colisao com o chao principal ---
       Clamp direto: se o jogador passou do chao por qualquer motivo (velocidade
       alta, tunneling), corrige imediatamente. Nao depende de y_anterior nem
       de flags — funciona a qualquer velocidade de queda. */
    if (!player.no_chao) {
        if (esta_sobre_gap(player.x, PLAYER_W)) {
            /* buraco: cai; reposiciona ao sair da tela */
            if (player.y > SCREEN_HEIGHT + 20) {
                player.vidas--;
                player.x = (player.x > 100) ? player.x - 60 : 20;
                player.y = GROUND_Y - PLAYER_H;
                player.vy = 0;
                player.invulneravel = INVULNERABLE_FRAMES;
            }
        } else if (player.y + PLAYER_H >= GROUND_Y) {
            /* clamp: fixa os pes exatamente no chao, sem oscilacao */
            player.y = GROUND_Y - PLAYER_H;
            player.vy = 0;
            player.no_chao = 1;

            /* lava: dano e quica */
            if (esta_sobre_lava(player.x, PLAYER_W) && player.invulneravel == 0) {
                player.vidas--;
                player.vy = JUMP_VELOCITY / 2;
                player.no_chao = 0;
                player.invulneravel = INVULNERABLE_FRAMES;
            }
        }
    }

    /* --- atualiza camera (segue o jogador, mantendo-o centralizado) --- */
    camera_x = player.x - SCREEN_WIDTH / 2 + PLAYER_W / 2;
    if (camera_x < 0) camera_x = 0;
    if (camera_x > nivel_largura - SCREEN_WIDTH) camera_x = nivel_largura - SCREEN_WIDTH;

    /* --- contador de invencibilidade --- */
    if (player.invulneravel > 0) player.invulneravel--;

    /* --- coleta de tokens: verifica colisao simples com cada token disponivel --- */
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

    /* --- checa derrota --- */
    if (player.vidas <= 0) {
        estado = ESTADO_GAMEOVER;
    }

    /* --- checa vitoria: chegar a bandeira COM tokens suficientes ---
       se faltar tokens, o jogador e bloqueado e ve quantos ainda precisa */
    if (player.x >= BANDEIRA_X - PLAYER_W) {
        if (tokens_coletados >= TOKENS_PARA_VENCER) {
            estado = ESTADO_VITORIA;
        } else {
            player.x = BANDEIRA_X - PLAYER_W - 1;  /* bloqueia ate coletar */
        }
    }
}

/* Atualiza projeteis: movimento, alcance fixo e colisao com inimigos */
void atualiza_projeteis(void) {
    int i, j;
    for (i = 0; i < MAX_PROJETEIS; i++) {
        if (!projeteis[i].ativo) continue;

        projeteis[i].x += projeteis[i].vx;
        projeteis[i].dist_percorrida += PROJETIL_SPEED;

        if (projeteis[i].dist_percorrida >= PROJETIL_ALCANCE) {
            projeteis[i].ativo = 0; continue;
        }
        if (projeteis[i].x < camera_x - 10 || projeteis[i].x > camera_x + SCREEN_WIDTH + 10) {
            projeteis[i].ativo = 0; continue;
        }

        if (projeteis[i].do_inimigo) {
            /* projetil inimigo: verifica colisao com o JOGADOR */
            if (player.invulneravel == 0 &&
                colide(projeteis[i].x, projeteis[i].y, PROJETIL_W, PROJETIL_H,
                       player.x, player.y, PLAYER_W, PLAYER_H)) {
                player.vidas--;
                player.invulneravel = INVULNERABLE_FRAMES;
                projeteis[i].ativo = 0;
            }
        } else {
            /* projetil do jogador: verifica colisao com INIMIGOS */
            for (j = 0; j < MAX_INIMIGOS; j++) {
                if (!inimigos[j].vivo) continue;
                if (colide(projeteis[i].x, projeteis[i].y, PROJETIL_W, PROJETIL_H,
                           inimigos[j].x, inimigos[j].y, inimigos[j].w, inimigos[j].h)) {
                    inimigos[j].vida--;
                    projeteis[i].ativo = 0;
                    if (inimigos[j].vida <= 0) {
                        inimigos[j].vivo = 0;
                    }
                    break;
                }
            }
        }
    }
}

/* Atualiza inimigos: checa colisao com o jogador (causa dano se nocivo) */
/* Calcula a distancia absoluta entre dois pontos no eixo x */
int distancia_x(int a, int b) {
    return (a > b) ? (a - b) : (b - a);
}

/* Move um inimigo de acordo com seu comportamento (patrulha ou perseguicao).
 * Inimigos do tipo COMPORTAMENTO_PARADO nao se movem. */
void move_inimigo(Inimigo *e) {
    if (e->comportamento == COMPORTAMENTO_PATRULHA) {
        /* anda entre patrulha_min_x e patrulha_max_x, invertendo direcao nos limites */
        if (e->vx == 0) {
            e->vx = INIMIGO_VELOCIDADE_PATRULHA; /* comeca andando para a direita */
        }
        e->x += e->vx;
        if (e->x <= e->patrulha_min_x) {
            e->x = e->patrulha_min_x;
            e->vx = INIMIGO_VELOCIDADE_PATRULHA;
            e->direcao = FACING_RIGHT;
        } else if (e->x >= e->patrulha_max_x) {
            e->x = e->patrulha_max_x;
            e->vx = -INIMIGO_VELOCIDADE_PATRULHA;
            e->direcao = FACING_LEFT;
        }
    } else if (e->comportamento == COMPORTAMENTO_PERSEGUICAO) {
        /* so persegue se o jogador estiver dentro da distancia de deteccao */
        if (distancia_x(player.x, e->x) <= DISTANCIA_DETECCAO) {
            if (player.x < e->x) {
                e->x -= INIMIGO_VELOCIDADE_PERSEGUICAO;
                e->direcao = FACING_LEFT;
            } else if (player.x > e->x) {
                e->x += INIMIGO_VELOCIDADE_PERSEGUICAO;
                e->direcao = FACING_RIGHT;
            }
        }
    }
    /* COMPORTAMENTO_PARADO: nao faz nada, fica parado no lugar */
}

void atualiza_inimigos(void) {
    int i;
    for (i = 0; i < MAX_INIMIGOS; i++) {
        if (!inimigos[i].vivo) continue;

        move_inimigo(&inimigos[i]);

        /* --- atiradores (tipo 2) e chefe (tipo 3): disparam ao detectar o jogador --- */
        if ((inimigos[i].tipo == 2 || inimigos[i].tipo == 3) &&
            distancia_x(player.x, inimigos[i].x) <= DISTANCIA_DETECCAO) {
            if (inimigos[i].cooldown_tiro <= 0) {
                dispara_projetil_inimigo(&inimigos[i]);
                /* chefe atira mais rapido */
                inimigos[i].cooldown_tiro = (inimigos[i].tipo == 3)
                    ? COOLDOWN_TIRO_INIMIGO / 2
                    : COOLDOWN_TIRO_INIMIGO;
            }
        }
        if (inimigos[i].cooldown_tiro > 0) inimigos[i].cooldown_tiro--;

        /* --- colisao por contato (dano ao tocar jogador) --- */
        if (colide(player.x, player.y, PLAYER_W, PLAYER_H,
                   inimigos[i].x, inimigos[i].y, inimigos[i].w, inimigos[i].h)) {
            /* tipo 2 (atirador) e tipo 3 (chefe) tambem danam por contato */
            if ((inimigos[i].tipo == 1 || inimigos[i].tipo == 2 || inimigos[i].tipo == 3)
                && player.invulneravel == 0) {
                player.vidas--;
                player.invulneravel = INVULNERABLE_FRAMES;
                if (player.x < inimigos[i].x) player.x -= 10;
                else                           player.x += 10;
            }
        }
    }
}

/* Atualiza todo o estado do jogo em um frame */
void atualiza_estado(void) {
    /* so atualiza fisica/IA quando ESTADO_JOGANDO; cobre tambem ESTADO_PAUSADO,
       ESTADO_GAMEOVER e ESTADO_VITORIA, que ficam "congelados" */
    if (estado != ESTADO_JOGANDO) return;

    atualiza_jogador();
    atualiza_projeteis();
    atualiza_inimigos();
}

/* ============================================================================
 * RENDERIZACAO
 * ============================================================================
 */

/* Desenha um "X" grande de game over usando retangulos (texto simples por blocos) */
void desenha_tela_gameover(void) {
    limpa_tela(COLOR_BLACK);
    /* moldura vermelha simples para indicar derrota */
    desenha_retangulo(60, 90, 200, 60, 0x4000);
    desenha_retangulo(70, 100, 180, 40, COLOR_BLACK);
    /* "X" no centro */
    int i;
    for (i = 0; i < 30; i++) {
        desenha_pixel(145 + i, 105 + i, 0xF800);
        desenha_pixel(145 + i, 135 - i, 0xF800);
    }
}

void desenha_tela_vitoria(void) {
    limpa_tela(COLOR_SKY);
    desenha_retangulo(60, 90, 200, 60, COLOR_TREASURE);
    desenha_retangulo(70, 100, 180, 40, COLOR_SKY);
    /* estrela simples no centro para indicar vitoria */
    desenha_retangulo(150, 110, 20, 20, COLOR_TREASURE);
}

/* Desenha a tela de pausa/configuracoes: o cenario congelado por baixo,
 * com uma sobreposicao escura, icone de pausa (duas barras) e instrucoes. */
void desenha_tela_pausa(void) {
    int x, y;

    /* desenha o cenario do jogo "congelado" (ultimo estado antes da pausa) */
    desenha_cenario();
    desenha_inimigos();
    desenha_projeteis();
    desenha_jogador();

    /* overlay escuro semi-transparente simulado: desenha uma grade de pixels
     * pretos espacados, dando efeito de escurecimento sem precisar de alpha
     * blending real (nao suportado pelo controlador de video RGB565) */
    for (y = 0; y < SCREEN_HEIGHT; y += 2) {
        for (x = 0; x < SCREEN_WIDTH; x += 2) {
            desenha_pixel(x, y, COLOR_BLACK);
        }
    }

    /* caixa de menu central */
    desenha_retangulo(70, 70, 180, 100, COLOR_STONE_DARK);
    desenha_retangulo(78, 78, 164, 84, COLOR_BLACK);

    /* icone de pausa: duas barras verticais */
    desenha_retangulo(140, 90, 12, 30, COLOR_WHITE);
    desenha_retangulo(160, 90, 12, 30, COLOR_WHITE);

    /* "linhas" decorativas representando o texto de instrucoes (sem fonte de texto disponivel):
     * SW0 para baixo = continuar | R = reiniciar */
    desenha_retangulo(90, 130, 140, 4, COLOR_TREASURE);   /* linha = "SW0: continuar" */
    desenha_retangulo(90, 142, 100, 4, COLOR_ENEMY_EYE);  /* linha = "R: reiniciar" */
}

/* Renderiza um frame completo do jogo no back buffer */
void renderiza_cena(void) {
    if (estado == ESTADO_GAMEOVER) {
        desenha_tela_gameover();
        return;
    }
    if (estado == ESTADO_VITORIA) {
        desenha_tela_vitoria();
        return;
    }
    if (estado == ESTADO_PAUSADO) {
        desenha_tela_pausa();
        return;
    }

    /* nao precisa de limpa_tela() aqui: desenha_cenario() ja pinta o ceu
       inteiro (por zona) e o chao inteiro, cobrindo toda a tela */
    desenha_cenario();
    desenha_plataformas();
    desenha_bandeira();
    desenha_tokens();
    desenha_inimigos();
    desenha_projeteis();
    desenha_jogador();
}

/* ============================================================================
 * ENTRADA: leitura unificada (teclado + chave) - chamada a cada frame
 * ============================================================================
 */

/* Processa a chave SW0 (pausa) e a tecla R (reiniciar).
 * Usa deteccao de borda para a chave: so alterna o estado quando a chave
 * MUDA de desligada para ligada, evitando entrar/sair da pausa repetidamente
 * a cada frame enquanto a chave permanece fisicamente ligada. */
void processa_pausa_e_reinicio(void) {
    static int chave_anterior = 0;
    int chave_atual = chave_sw0_ligada();

    /* borda de subida: chave acabou de ser ligada */
    if (chave_atual && !chave_anterior) {
        if (estado == ESTADO_JOGANDO) {
            estado = ESTADO_PAUSADO;
        } else if (estado == ESTADO_PAUSADO) {
            estado = ESTADO_JOGANDO;
        }
        /* em ESTADO_GAMEOVER/ESTADO_VITORIA a chave nao tem efeito:
           essas telas ja se resolvem sozinhas apos a pausa temporizada */
    }
    chave_anterior = chave_atual;

    /* tecla R reinicia o jogo manualmente, mas so faz sentido durante a pausa
       (durante o jogo normal, R nao deve interromper a partida sem querer) */
    if (estado == ESTADO_PAUSADO && tecla_reiniciar) {
        inicializa_jogo();
        tecla_reiniciar = 0; /* consome o evento para nao reiniciar de novo no proximo frame */
    }
}

void le_entrada(void) {
    processa_teclado();
    processa_pausa_e_reinicio();
}

/* Pequeno atraso para controlar a velocidade do jogo (alternativa simples a um timer) */
void atraso_frame(void) {
    volatile int i;
    for (i = 0; i < 8000; i++) {
        /* busy-wait simples; valor reduzido (era 30000) para o jogo rodar mais rapido.
           ajuste este numero se o jogo estiver rapido/lento demais ao rodar no CPUlator
           (a velocidade de simulacao varia de maquina pra maquina) */
    }
}

/* ============================================================================
 * FUNCAO PRINCIPAL (main) - LACO DO JOGO
 * ============================================================================
 */
int main(void) {
    inicializa_video();
    inicializa_jogo();

    while (1) {
        le_entrada();              /* lerEntrada()       */
        atualiza_estado();         /* atualizarEstado()  */
        renderiza_cena();          /* renderizarCena()   */

        atualiza_pontuacao_display(tokens_coletados);  /* HEX3-HEX0: tokens coletados (0-10) */
        atualiza_vidas_display(player.vidas);
        atualiza_vidas_leds(player.vidas);

        troca_buffers();           /* exibe o frame desenhado (double buffering) */

        /* le o teclado de novo logo apos o VSYNC: troca_buffers() fica em busy-wait
           esperando a sincronizacao, e novos bytes do PS/2 podem chegar nesse meio
           tempo (especialmente com uma tecla segurada, que gera "repeat" continuo).
           sem isso, a FIFO de 8 posicoes do PS/2 pode encher antes da proxima
           chamada de le_entrada() no topo do laco, causando overflow. */
        le_entrada();

        /* se o jogo terminou (vitoria/derrota), aguarda um tempo e reinicia.
           IMPORTANTE: ESTADO_PAUSADO fica de fora dessa checagem -- a pausa deve
           durar enquanto a chave SW0 estiver ligada, sem reiniciar sozinha. */
        if (estado == ESTADO_GAMEOVER || estado == ESTADO_VITORIA) {
            int espera;
            for (espera = 0; espera < 30; espera++) {
                le_entrada();      /* continua drenando o PS/2 mesmo na tela de fim de jogo */
                atraso_frame();
            }
            inicializa_jogo();
        }

        /* nao ha atraso extra aqui de proposito: troca_buffers() ja sincroniza
           com o VSYNC da tela (~60 Hz), entao a taxa de quadros ja fica regulada.
           um atraso adicional so aumentava o tempo entre leituras do PS/2 sem
           necessidade, contribuindo para o overflow da FIFO ao segurar teclas. */
    }

    return 0;
}
