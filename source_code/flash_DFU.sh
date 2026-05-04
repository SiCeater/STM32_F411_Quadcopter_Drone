#!/bin/bash
set -euo pipefail
IFS=$'\n\t'

#######################################
# Configuration
#######################################

CUBE_CLI="/Applications/STMicroelectronics/STM32CubeProgrammer.app/Contents/MacOs/bin/STM32_Programmer_CLI"
ELF_PATH="${workspaceFolder}/build/Debug/F411_MX_CONF.elf"

STM32_VID="0483"
STM32_DFU_PID="df11"

#######################################
# Fonctions utilitaires
#######################################

die() {
    echo "/!\ $1" >&2
    exit 1
}

info() {
    echo "(i)  $1"
}

ok() {
    echo "(OK) $1"
}

#######################################
# Pré-checks
#######################################

command -v "$CUBE_CLI" >/dev/null 2>&1 \
    || die "STM32_Programmer_CLI introuvable (PATH ?)"

[ -f "$ELF_PATH" ] \
    || die "ELF introuvable : $ELF_PATH"

info "ELF détecté : $ELF_PATH"

#######################################
# Détection DFU
#######################################

info "Recherche du port..."

DFU_PORTS=$(
    "$CUBE_CLI" -l |
    grep -Ei "$STM32_VID.*$STM32_DFU_PID" |
    awk '{print $1}' || true
)

[ -n "$DFU_PORTS" ] \
    || die "Aucun périphérique STM32 DFU détecté (no BOOT0 mode)"

DFU_COUNT=$(echo "$DFU_PORTS" | wc -l | tr -d ' ')

if [ "$DFU_COUNT" -gt 1 ]; then
    echo "/!\  Plusieurs périphériques DFU détectés :"
    echo "$DFU_PORTS"
    die "Connexion ambiguë – débranche les autres cartes"
fi

DFU_PORT="$DFU_PORTS"
ok "STM32 DFU détecté sur $DFU_PORT"

#######################################
# Flash
#######################################

info "flash DFU..."

"$CUBE_CLI" \
    -c port="$DFU_PORT" \
    -w "$ELF_PATH" \
    -v \
    -rst

ok "Flash terminé"
