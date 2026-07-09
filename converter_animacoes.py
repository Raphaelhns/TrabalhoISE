"""
Converte frames selecionados da spritesheet do Rambro para arrays C (RGB565).
Normaliza todos para o mesmo tamanho e marca pixels de fundo como transparentes.
"""
from PIL import Image
import os

TRANSPARENT_COLOR = 0xF81F  # Magenta = cor "invisivel"

def rgb_to_565(r, g, b):
    return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)

def eh_fundo(r, g, b, limiar=240):
    return r > limiar and g > limiar and b > limiar

def converter_frames(pasta_sprites, linha, frame_inicio, frame_fim, nome_anim, largura_final, altura_final, arquivo_saida):
    """Converte uma sequencia de frames em arrays C."""
    frames = []
    
    # Encontrar os arquivos correspondentes
    for frame_idx in range(frame_inicio, frame_fim + 1):
        # Procurar arquivo que comeca com o padrao correto
        prefixo = f"linha{linha:02d}_frame{frame_idx:02d}_"
        encontrado = None
        for f in os.listdir(pasta_sprites):
            if f.startswith(prefixo) and f.endswith(".png"):
                encontrado = f
                break
        if encontrado:
            frames.append((frame_idx, os.path.join(pasta_sprites, encontrado)))
        else:
            print(f"  AVISO: Frame {prefixo}*.png nao encontrado, pulando...")
    
    print(f"  {nome_anim}: {len(frames)} frames encontrados")
    
    with open(arquivo_saida, "a") as f:
        f.write(f"/* === Animacao: {nome_anim} === */\n")
        f.write(f"#define {nome_anim.upper()}_FRAMES {len(frames)}\n\n")
        
        for i, (idx, caminho) in enumerate(frames):
            img = Image.open(caminho).convert("RGB")
            orig_w, orig_h = img.size
            
            # Centralizar o sprite no tamanho padrao
            offset_x = (largura_final - orig_w) // 2
            offset_y = altura_final - orig_h  # Alinhar pela base (pes)
            
            array_name = f"{nome_anim}_frame{i}"
            f.write(f"static const unsigned short {array_name}[{largura_final * altura_final}] = {{\n")
            
            for y in range(altura_final):
                f.write("    ")
                for x in range(largura_final):
                    src_x = x - offset_x
                    src_y = y - offset_y
                    
                    if 0 <= src_x < orig_w and 0 <= src_y < orig_h:
                        r, g, b = img.getpixel((src_x, src_y))
                        if eh_fundo(r, g, b):
                            valor = TRANSPARENT_COLOR
                        else:
                            valor = rgb_to_565(r, g, b)
                    else:
                        valor = TRANSPARENT_COLOR
                    
                    f.write(f"0x{valor:04X}")
                    if y * largura_final + x < largura_final * altura_final - 1:
                        f.write(",")
                f.write("\n")
            
            f.write("};\n\n")
        
        # Array de ponteiros para os frames
        f.write(f"static const unsigned short *{nome_anim}_anim[{len(frames)}] = {{\n")
        for i in range(len(frames)):
            f.write(f"    {nome_anim}_frame{i}")
            if i < len(frames) - 1:
                f.write(",")
            f.write("\n")
        f.write("};\n\n")

def main():
    pasta = "src/entidades/sprites_extraidos"
    saida = "src/entidades/player_sprites.h"
    
    # Tamanho padrao para todos os frames (baseado no maior sprite + margem)
    SPRITE_W = 24
    SPRITE_H = 24
    
    with open(saida, "w") as f:
        f.write("#ifndef PLAYER_SPRITES_H\n")
        f.write("#define PLAYER_SPRITES_H\n\n")
        f.write(f"#define SPRITE_W {SPRITE_W}\n")
        f.write(f"#define SPRITE_H {SPRITE_H}\n")
        f.write(f"#define SPRITE_TRANSPARENT 0xF81F\n\n")
    
    # Correndo: Linha 1, frames 0-15
    print("Convertendo animacao de corrida...")
    converter_frames(pasta, 1, 0, 15, "run", SPRITE_W, SPRITE_H, saida)
    
    # Atirando: Linha 4, frames 8-15
    print("Convertendo animacao de tiro...")
    converter_frames(pasta, 4, 8, 15, "shoot", SPRITE_W, SPRITE_H, saida)
    
    # Parado (idle): Linha 3, frame 0 (apenas 1 frame estatico)
    print("Convertendo animacao idle...")
    converter_frames(pasta, 3, 0, 0, "idle", SPRITE_W, SPRITE_H, saida)
    
    # Intro (inicio do jogo): Linha 10, frames 0-7
    print("Convertendo animacao intro...")
    converter_frames(pasta, 10, 0, 7, "intro", SPRITE_W, SPRITE_H, saida)
    
    # Pulando: Linha 6, frames 7-15
    print("Convertendo animacao de pulo...")
    converter_frames(pasta, 6, 7, 15, "jump", SPRITE_W, SPRITE_H, saida)
    
    with open(saida, "a") as f:
        f.write("#endif\n")
    
    print(f"\nTodos os sprites salvos em: {saida}")

if __name__ == "__main__":
    main()
