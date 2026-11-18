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
  echo "usage: $0 qemu|solution|check|gdb NN" >&2
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
if [[ ! -d "$ROOT/build" ]]; then
  cmake -B "$ROOT/build" -G Ninja -DCMAKE_TOOLCHAIN_FILE="$ROOT/cmake/gcc-arm-none-eabi.cmake" -S "$ROOT"
fi
case $CMD in
  qemu)     cmake --build "$ROOT/build" --target "qemu-${ID}" ;;
  solution) cmake --build "$ROOT/build" --target "qemu-${ID}-solution" ;;
  check)    cmake --build "$ROOT/build" --target "check-${ID}" ;;
  gdb)
    cmake --build "$ROOT/build" --target "lab_${ID}"
    ELF=$(find "$ROOT/build" \( -name "lab_${ID}" -o -name "lab_${ID}.elf" \) -type f | head -1)
    if [[ -z "$ELF" ]]; then
      echo "ELF not found for lab_${ID}" >&2
      exit 1
    fi
    "$QEMU" -M netduinoplus2 -display none -serial stdio -s -S -kernel "$ELF" &
    QPID=$!
    trap 'kill $QPID 2>/dev/null || true' EXIT
    GDB=$(command -v gdb-multiarch || command -v arm-none-eabi-gdb)
    "$GDB" -ex "target remote :1234" "$ELF"
    ;;
  *) echo "usage: $0 qemu|solution|check|gdb NN" >&2; exit 1 ;;
esac