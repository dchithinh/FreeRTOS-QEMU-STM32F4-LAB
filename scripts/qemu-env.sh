# Shared QEMU lookup. Source after ROOT is set. Do not execute.
# Prefer a binary that actually supports -M netduinoplus2 (Ubuntu 20.04
# qemu-system-arm 4.2 only has netduino2).

FREERTOS_QEMU_MACHINE=netduinoplus2

qemu_supports_machine() {
  local bin=$1
  [[ -x "$bin" ]] || return 1
  "$bin" -machine help 2>/dev/null | grep -q "^${FREERTOS_QEMU_MACHINE}[[:space:]]"
}

find_freertos_qemu() {
  local candidates=()
  if [[ -n "${FREERTOS_QEMU:-}" ]]; then
    candidates+=("$FREERTOS_QEMU")
  fi
  if command -v qemu-system-arm >/dev/null 2>&1; then
    candidates+=("$(command -v qemu-system-arm)")
  fi
  candidates+=("$ROOT/.tools/bin/qemu-system-arm")
  candidates+=("/tmp/qemu-8.2.2/build/qemu-system-arm")

  local q
  for q in "${candidates[@]}"; do
    if qemu_supports_machine "$q"; then
      printf '%s\n' "$q"
      return 0
    fi
  done

  echo "No qemu-system-arm with machine '${FREERTOS_QEMU_MACHINE}'." >&2
  echo "Ubuntu 20.04's qemu 4.2 only has netduino2 (Cortex-M3)." >&2
  echo "Install QEMU 5+ (Ubuntu 22.04+ package) or run: ./scripts/install-qemu.sh" >&2
  echo "Or set FREERTOS_QEMU to a suitable qemu-system-arm." >&2
  return 1
}
