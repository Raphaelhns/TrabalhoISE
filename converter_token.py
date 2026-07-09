"""
Converte src/entidades/token_sprite.png (bau + gemas) para um array C RGB565,
redimensionado para o tamanho do token no jogo (TOKEN_W x TOKEN_H).
Usa o canal alpha da imagem para decidir transparencia (chroma-key magenta).
"""
from PIL import Image

TRANSPARENT_COLOR = 0xF81F

def rgb_to_565(r, g, b):
    return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)

def main():
    TOKEN_W = 16
    TOKEN_H = 16

    img = Image.open("src/entidades/token_sprite.png").convert("RGBA")
    img = img.resize((TOKEN_W, TOKEN_H), Image.LANCZOS)

    saida = "src/entidades/token_sprites.h"
    with open(saida, "w") as f:
        f.write("#ifndef TOKEN_SPRITES_H\n")
        f.write("#define TOKEN_SPRITES_H\n\n")
        f.write(f"#define TOKEN_SPRITE_W {TOKEN_W}\n")
        f.write(f"#define TOKEN_SPRITE_H {TOKEN_H}\n")
        f.write(f"#define TOKEN_TRANSPARENT 0x{TRANSPARENT_COLOR:04X}\n\n")

        f.write(f"static const unsigned short token_sprite[{TOKEN_W * TOKEN_H}] = {{\n")
        pixels = list(img.getdata())
        for i, (r, g, b, a) in enumerate(pixels):
            if i % TOKEN_W == 0:
                f.write("    ")
            if a < 128:
                valor = TRANSPARENT_COLOR
            else:
                valor = rgb_to_565(r, g, b)
            f.write(f"0x{valor:04X}")
            if i < len(pixels) - 1:
                f.write(",")
            if i % TOKEN_W == TOKEN_W - 1 or i == len(pixels) - 1:
                f.write("\n")
            else:
                f.write(" ")
        f.write("};\n\n")
        f.write("#endif\n")

    print(f"Header salvo em: {saida}")

if __name__ == "__main__":
    main()
