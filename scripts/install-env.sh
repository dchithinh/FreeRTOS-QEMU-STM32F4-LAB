#!/usr/bin/env bash
set -euo pipefail
export DEBIAN_FRONTEND=noninteractive
sudo apt-get update
sudo apt-get install -y cmake ninja-build gcc-arm-none-eabi qemu-system-arm gdb-multiarch
need() {
  if ! command -v "$1" >/dev/null 2>&1; then
    echo "missing: $1 (package: $2)" >&2
    exit 1
  fi
}
need cmake cmake
need ninja ninja-build
need arm-none-eabi-gcc gcc-arm-none-eabi
need qemu-system-arm qemu-system-arm
if ! command -v gdb-multiarch >/dev/null 2>&1 && ! command -v arm-none-eabi-gdb >/dev/null 2>&1; then
  echo "missing: gdb-multiarch or arm-none-eabi-gdb" >&2
  exit 1
fi
cmake --version | head -1
arm-none-eabi-gcc --version | head -1
qemu-system-arm --version | head -1
# Ubuntu 20.04 qemu 4.2 has no netduinoplus2 (need QEMU 5+).
# shellcheck source=qemu-env.sh
ROOT=$(cd "$(dirname "$0")/.." && pwd)
source "$ROOT/scripts/qemu-env.sh"
if ! find_freertos_qemu >/dev/null; then
  echo "qemu-system-arm is too old for -M netduinoplus2. Run: ./scripts/install-qemu.sh" >&2
  exit 1
fi
echo "env OK"