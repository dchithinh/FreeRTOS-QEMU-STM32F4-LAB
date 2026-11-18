#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "$0")/.." && pwd)
# shellcheck source=qemu-env.sh
source "$ROOT/scripts/qemu-env.sh"
ELF=$1
EXPECT=$2
QEMU=$(find_freertos_qemu)
OUT=$(mktemp)
TIMEOUT=8
set +e
timeout --signal=KILL "${TIMEOUT}s" "$QEMU" -M "$FREERTOS_QEMU_MACHINE" -display none -serial stdio -kernel "$ELF" >"$OUT" 2>&1
set -e
if ! tr -d '\r' <"$OUT" | grep -q '^DONE$'; then
  echo "timeout or missing DONE" >&2
  cat "$OUT" >&2
  rm -f "$OUT"
  exit 1
fi
awk '1; /^DONE$/{exit}' "$OUT" | tr -d '\r' > "${OUT}.norm"
tr -d '\r' < "$EXPECT" > "${OUT}.exp"
diff -u "${OUT}.exp" "${OUT}.norm"
rm -f "$OUT" "${OUT}.norm" "${OUT}.exp"