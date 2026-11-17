#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "$0")/.." && pwd)
ELF=$1
if command -v qemu-system-arm >/dev/null 2>&1; then
  QEMU=qemu-system-arm
elif [[ -x "$ROOT/.tools/bin/qemu-system-arm" ]]; then
  QEMU="$ROOT/.tools/bin/qemu-system-arm"
elif [[ -x /tmp/qemu-8.2.2/build/qemu-system-arm ]]; then
  QEMU=/tmp/qemu-8.2.2/build/qemu-system-arm
else
  echo "qemu-system-arm not found (PATH, $ROOT/.tools/bin, or /tmp/qemu-8.2.2/build)" >&2
  exit 1
fi
exec "$QEMU" -M netduinoplus2 -display none -serial stdio -kernel "$ELF"