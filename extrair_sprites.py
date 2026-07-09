"""
Extrai sprites individuais de uma spritesheet do Broforce.
Detecta regioes nao-brancas e agrupa em bounding boxes por linha.
"""
from PIL import Image
import os

def eh_fundo(pixel, limiar=240):
    """Verifica se o pixel e fundo (branco ou quase branco)."""
    if isinstance(pixel, tuple):
        if len(pixel) == 4 and pixel[3] < 30:  # RGBA transparente
            return True
        return pixel[0] > limiar and pixel[1] > limiar and pixel[2] > limiar
    return pixel > limiar

def encontra_sprites(img, limiar_fundo=240, min_tamanho=8):
    """Encontra bounding boxes de todos os sprites na imagem."""
    largura, altura = img.size
    pixels = img.load()
    
    # Criar mascara binaria (True = sprite, False = fundo)
    visitado = [[False]*largura for _ in range(altura)]
    sprites = []
    
    for y in range(altura):
        for x in range(largura):
            if visitado[y][x]:
                continue
            pixel = pixels[x, y]
            if eh_fundo(pixel, limiar_fundo):
                visitado[y][x] = True
                continue
            
            # Flood fill para encontrar o bounding box do sprite
            fila = [(x, y)]
            min_x, max_x = x, x
            min_y, max_y = y, y
            visitado[y][x] = True
            
            while fila:
                cx, cy = fila.pop(0)
                for dx, dy in [(-1,0),(1,0),(0,-1),(0,1),(-1,-1),(1,-1),(-1,1),(1,1)]:
                    nx, ny = cx+dx, cy+dy
                    if 0 <= nx < largura and 0 <= ny < altura and not visitado[ny][nx]:
                        visitado[ny][nx] = True
                        p = pixels[nx, ny]
                        if not eh_fundo(p, limiar_fundo):
                            fila.append((nx, ny))
                            min_x = min(min_x, nx)
                            max_x = max(max_x, nx)
                            min_y = min(min_y, ny)
                            max_y = max(max_y, ny)
            
            w = max_x - min_x + 1
            h = max_y - min_y + 1
            if w >= min_tamanho and h >= min_tamanho:
                sprites.append((min_x, min_y, w, h))
    
    return sprites

def agrupa_por_linha(sprites, tolerancia=5):
    """Agrupa sprites que estao na mesma linha horizontal."""
    sprites_sorted = sorted(sprites, key=lambda s: (s[1], s[0]))
    linhas = []
    linha_atual = [sprites_sorted[0]]
    y_atual = sprites_sorted[0][1]
    
    for s in sprites_sorted[1:]:
        if abs(s[1] - y_atual) < tolerancia:
            linha_atual.append(s)
        else:
            linhas.append(sorted(linha_atual, key=lambda s: s[0]))
            linha_atual = [s]
            y_atual = s[1]
    linhas.append(sorted(linha_atual, key=lambda s: s[0]))
    
    return linhas

def main():
    img = Image.open("src/entidades/rambro_spritesheet.png").convert("RGBA")
    print(f"Spritesheet: {img.size[0]}x{img.size[1]}")
    print("Detectando sprites... (isso pode levar alguns segundos)")
    
    sprites = encontra_sprites(img, limiar_fundo=240, min_tamanho=10)
    print(f"\nTotal de sprites encontrados: {len(sprites)}")
    
    linhas = agrupa_por_linha(sprites, tolerancia=10)
    print(f"Total de linhas de animacao: {len(linhas)}")
    
    # Criar pasta de saida
    saida_dir = "src/entidades/sprites_extraidos"
    os.makedirs(saida_dir, exist_ok=True)
    
    for i, linha in enumerate(linhas):
        print(f"\nLinha {i}: {len(linha)} frames")
        for j, (x, y, w, h) in enumerate(linha):
            print(f"  Frame {j}: pos=({x},{y}), tamanho={w}x{h}")
            frame = img.crop((x, y, x+w, y+h))
            frame.save(f"{saida_dir}/linha{i:02d}_frame{j:02d}_{w}x{h}.png")
    
    print(f"\nTodos os frames salvos em: {saida_dir}/")
    print("\nAnalise as imagens extraidas e me diga quais linhas correspondem a:")
    print("  - Parado (idle)")
    print("  - Correndo (run)")
    print("  - Pulando (jump)")
    print("  - Atirando (shoot)")

if __name__ == "__main__":
    main()
