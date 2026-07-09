"""
Extrai tiles de lava do tileset do Super Mario World.
Usa tiles pre-selecionados manualmente (revisados visualmente em src/ambiente/lava_tiles/):
  - Topo animado (efeito de lava se mexendo): superficie_12, 13, 14, 17
  - Corpo solido (bloco vermelho de baixo): corpo_0
Converte para arrays C RGB565 com transparencia por chroma-key verde.
"""
from PIL import Image

def rgb_to_565(r, g, b):
    return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)

def eh_fundo(r, g, b, limiar=200):
    """Detecta fundo verde do tileset."""
    return g > limiar and r < 100 and b < 100

def converter_tile(img, nome, arquivo):
    """Converte um tile para array C RGB565."""
    w, h = img.size
    pixels = list(img.getdata())

    with open(arquivo, "a") as f:
        f.write(f"static const unsigned short {nome}[{w * h}] = {{\n")
        for i, (r, g, b) in enumerate(pixels):
            if i % 16 == 0:
                f.write("    ")
            if eh_fundo(r, g, b):
                valor = 0xF81F  # transparente
            else:
                valor = rgb_to_565(r, g, b)
            f.write(f"0x{valor:04X}")
            if i < w * h - 1:
                f.write(",")
            if i % 16 == 15 or i == w * h - 1:
                f.write("\n")
            else:
                f.write(" ")
        f.write("};\n\n")

def main():
    img = Image.open("src/ambiente/lava.png").convert("RGB")
    saida = "src/ambiente/lava_sprites.h"

    TILE_SIZE = 16

    # Indices escolhidos apos revisar src/ambiente/lava_tiles/*.png:
    # tiles de superficie fora dessa lista eram artefatos (texto/swatches
    # do tileset original) ou fragmentos parciais demais para animar bem.
    SUPERFICIE_ESCOLHIDAS = [12, 13, 14, 17]
    CORPO_ESCOLHIDO = 0

    def tile_at(col_idx_lista, kind):
        """Recalcula a posicao (x,y) de um tile a partir do indice de deteccao
        original (mesma ordem de varredura usada na extracao inicial)."""
        detectados = []
        for tile_y in range(0, img.size[1] - TILE_SIZE + 1, TILE_SIZE):
            for tile_x in range(0, img.size[0] - TILE_SIZE + 1, TILE_SIZE):
                tile = img.crop((tile_x, tile_y, tile_x + TILE_SIZE, tile_y + TILE_SIZE))
                pixels = list(tile.getdata())
                nao_fundo = 0
                vermelho_total = 0
                for r, g, b in pixels:
                    if not eh_fundo(r, g, b):
                        nao_fundo += 1
                        if r > 150 and g < 100:
                            vermelho_total += 1
                if nao_fundo < TILE_SIZE * TILE_SIZE * 0.3:
                    continue
                prop_vermelho = vermelho_total / max(nao_fundo, 1)
                if tile_y < 80:
                    eh_corpo = prop_vermelho > 0.85
                    if (kind == "corpo") == eh_corpo:
                        detectados.append(tile)
        return detectados

    tiles_superficie = tile_at(None, "superficie")
    tiles_corpo = tile_at(None, "corpo")

    with open(saida, "w") as f:
        f.write("#ifndef LAVA_SPRITES_H\n")
        f.write("#define LAVA_SPRITES_H\n\n")
        f.write(f"#define LAVA_TILE_SIZE {TILE_SIZE}\n")
        f.write("#define LAVA_TRANSPARENT 0xF81F\n\n")

    n_frames = len(SUPERFICIE_ESCOLHIDAS)
    for i, idx in enumerate(SUPERFICIE_ESCOLHIDAS):
        converter_tile(tiles_superficie[idx], f"lava_topo_{i}", saida)

    with open(saida, "a") as f:
        f.write(f"#define LAVA_TOPO_FRAMES {n_frames}\n")
        f.write(f"static const unsigned short *lava_topo_anim[{n_frames}] = {{\n")
        for i in range(n_frames):
            f.write(f"    lava_topo_{i}")
            if i < n_frames - 1:
                f.write(",")
            f.write("\n")
        f.write("};\n\n")

    converter_tile(tiles_corpo[CORPO_ESCOLHIDO], "lava_corpo", saida)

    with open(saida, "a") as f:
        f.write("#endif\n")

    print(f"Header salvo em: {saida}")
    print(f"Frames de topo: {SUPERFICIE_ESCOLHIDAS} | Corpo: {CORPO_ESCOLHIDO}")

if __name__ == "__main__":
    main()
