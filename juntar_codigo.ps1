$arquivos_h = @(
    "src\core\constants.h",
    "src\core\types.h",
    "src\core\hardware.h",
    "src\entidades\player.h",
    "src\entidades\enemy.h",
    "src\entidades\projectiles.h",
    "src\ambiente\level.h",
    "src\core\game.h"
)

$arquivos_c = @(
    "src\core\hardware.c",
    "src\entidades\player.c",
    "src\entidades\enemy.c",
    "src\entidades\projectiles.c",
    "src\ambiente\level.c",
    "src\core\game.c",
    "src\core\main.c"
)

$saida = "jogo_completo.c"
Clear-Content $saida -ErrorAction SilentlyContinue

Add-Content $saida "#include <stdint.h>`n#include <stdlib.h>`n"

foreach ($arquivo in $arquivos_h + $arquivos_c) {
    Get-Content $arquivo | Where-Object { $_ -notmatch '^#include\s*"' } | Add-Content $saida
}
Write-Host "Arquivo $saida gerado com sucesso! Voce pode copiar tudo dele."
