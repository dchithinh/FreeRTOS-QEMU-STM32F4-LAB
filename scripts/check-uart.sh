#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "$0")/.." && pwd)
ELF=$1
EXPECT=$2
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
OUT=$(mktemp)
TIMEOUT=8
set +e
timeout --signal=KILL "${TIMEOUT}s" "$QEMU" -M netduinoplus2 -display none -serial stdio -kernel "$ELF" >"$OUT" 2>&1
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