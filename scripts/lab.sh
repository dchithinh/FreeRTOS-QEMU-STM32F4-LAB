#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "$0")/.." && pwd)
KERNEL_TASKS="$ROOT/third_party/FreeRTOS-Kernel/tasks.c"
if [[ ! -f "$KERNEL_TASKS" ]]; then
  echo "FreeRTOS kernel not found: $KERNEL_TASKS" >&2
  echo "Run: git submodule update --init --recursive" >&2
  exit 1
fi
if [[ $# -lt 2 ]]; then
  echo "usage: $0 qemu|solution|check|gdb|gdb-solution NN" >&2
  exit 1
fi
CMD=$1
ID=$(printf '%02d' "$((10#$2))")
shopt -s nullglob
lab_dirs=("$ROOT/labs/${ID}-"*)
shopt -u nullglob
if [[ ${#lab_dirs[@]} -eq 0 ]]; then
  echo "lab ${ID} not found (expected $ROOT/labs/${ID}-*)" >&2
  exit 1
fi
# shellcheck source=qemu-env.sh
source "$ROOT/scripts/qemu-env.sh"
QEMU=$(find_freertos_qemu)
if [[ ! -d "$ROOT/build" ]]; then
  cmake -B "$ROOT/build" -G Ninja -DCMAKE_TOOLCHAIN_FILE="$ROOT/cmake/gcc-arm-none-eabi.cmake" -S "$ROOT"
fi
case $CMD in
  qemu)     cmake --build "$ROOT/build" --target "qemu-${ID}" ;;
  solution) cmake --build "$ROOT/build" --target "qemu-${ID}-solution" ;;
  check)    cmake --build "$ROOT/build" --target "check-${ID}" ;;
  gdb|gdb-solution)
    if [[ $CMD == gdb-solution ]]; then
      TARGET="lab_${ID}_solution"
    else
      TARGET="lab_${ID}"
    fi
    cmake --build "$ROOT/build" --target "$TARGET"
    ELF=$(find "$ROOT/build" -name "$TARGET" -type f | head -1)
    if [[ -z "$ELF" ]]; then
      echo "ELF not found for $TARGET" >&2
      exit 1
    fi
    "$QEMU" -M "$FREERTOS_QEMU_MACHINE" -display none -serial stdio -s -S -kernel "$ELF" &
    QPID=$!
    trap 'kill $QPID 2>/dev/null || true' EXIT
    GDB=$(command -v gdb-multiarch || command -v arm-none-eabi-gdb)
    "$GDB" -ex "target remote :1234" "$ELF"
    ;;
  *) echo "usage: $0 qemu|solution|check|gdb|gdb-solution NN" >&2; exit 1 ;;
esac