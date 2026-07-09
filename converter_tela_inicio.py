"""
Converte src/ambiente/tela_inicio.png (arte widescreen, ja tem "START" desenhado)
para um fundo RGB565 320x240. Como a imagem original e 16:9 e a tela e 4:3,
corta as bordas laterais (mantendo o centro, onde ficam o personagem e o "START")
em vez de esticar e distorcer a arte.
"""
from PIL import Image

def rgb_to_565(r, g, b):
    return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)

def main():
    SCREEN_W = 320
    SCREEN_H = 240

    img = Image.open("src/ambiente/tela_inicio.png").convert("RGB")
    w, h = img.size

    alvo_w = h * SCREEN_W // SCREEN_H
    if alvo_w <= w:
        corte_x = (w - alvo_w) // 2
        img = img.crop((corte_x, 0, corte_x + alvo_w, h))
    else:
        alvo_h = w * SCREEN_H // SCREEN_W
        corte_y = (h - alvo_h) // 2
        img = img.crop((0, corte_y, w, corte_y + alvo_h))

    img = img.resize((SCREEN_W, SCREEN_H), Image.LANCZOS)

    saida = "src/ambiente/tela_inicio.h"
    with open(saida, "w") as f:
        f.write("#ifndef TELA_INICIO_H\n")
        f.write("#define TELA_INICIO_H\n\n")
        f.write(f"#define TELA_INICIO_W {SCREEN_W}\n")
        f.write(f"#define TELA_INICIO_ALTURA {SCREEN_H}\n\n")
        f.write(f"static const unsigned short tela_inicio[{SCREEN_W * SCREEN_H}] = {{\n")
        pixels = list(img.getdata())
        for i, (r, g, b) in enumerate(pixels):
            if i % SCREEN_W == 0:
                f.write("    ")
            valor = rgb_to_565(r, g, b)
            f.write(f"0x{valor:04X}")
            if i < len(pixels) - 1:
                f.write(",")
            if i % SCREEN_W == SCREEN_W - 1 or i == len(pixels) - 1:
                f.write("\n")
            else:
                f.write(" ")
        f.write("};\n\n")
        f.write("#endif\n")

    print(f"Header salvo em: {saida}")

if __name__ == "__main__":
    main()
