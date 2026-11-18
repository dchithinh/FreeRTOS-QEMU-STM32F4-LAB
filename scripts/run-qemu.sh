#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "$0")/.." && pwd)
# shellcheck source=qemu-env.sh
source "$ROOT/scripts/qemu-env.sh"
ELF=$1
QEMU=$(find_freertos_qemu)
exec "$QEMU" -M "$FREERTOS_QEMU_MACHINE" -display none -serial stdio -kernel "$ELF"