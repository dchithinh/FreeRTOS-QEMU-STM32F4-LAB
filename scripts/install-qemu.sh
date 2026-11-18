#!/usr/bin/env bash
# Put a QEMU with -M netduinoplus2 in .tools/bin (gitignored).
set -euo pipefail
ROOT=$(cd "$(dirname "$0")/.." && pwd)
# shellcheck source=qemu-env.sh
source "$ROOT/scripts/qemu-env.sh"

if QEMU=$(find_freertos_qemu 2>/dev/null); then
  echo "already usable: $QEMU"
  "$QEMU" --version | head -1
  exit 0
fi

DEST="$ROOT/.tools/bin/qemu-system-arm"
mkdir -p "$ROOT/.tools/bin"

# Reuse a locally built binary if present (not committed).
for src in \
  /tmp/qemu-8.2.2/build/qemu-system-arm \
  "$ROOT/.worktrees/feat-freertos-exercises/.tools/bin/qemu-system-arm"
do
  if qemu_supports_machine "$src"; then
    cp -a "$src" "$DEST"
    echo "installed $DEST (copied from $src)"
    "$DEST" --version | head -1
    exit 0
  fi
done

echo "No local QEMU 5+ binary found." >&2
echo "On Ubuntu 22.04+:  sudo apt-get install -y qemu-system-arm" >&2
echo "Or build QEMU 8.2 into .tools (needs ninja, meson, python3):" >&2
echo "  https://download.qemu.org/qemu-8.2.2.tar.xz" >&2
echo "  ./configure --target-list=arm-softmmu --disable-docs --prefix=$ROOT/.tools" >&2
exit 1
