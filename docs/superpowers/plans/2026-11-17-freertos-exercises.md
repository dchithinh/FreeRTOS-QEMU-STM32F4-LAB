# FreeRTOS QEMU Exercises Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Ship a CMake-built FreeRTOS kernel lab repo that boots on QEMU `netduinoplus2`, with 28 labs (src + solution), a BSP UART/IRQ API, and Phase 1 UART checks.

**Architecture:** One ARM Cortex-M4F firmware pipeline (startup, linker, UART, dedicated IRQ, FreeRTOS GCC/ARM_CM4F + heap_4). Each lab is two CMake executables (`lab_NN`, `lab_NN_solution`) sharing `bsp` + kernel; only `app.c` and `FreeRTOSConfig.h` differ. Host wrappers `scripts/lab.sh` / `run-qemu.sh` / `check-uart.sh` configure, run QEMU, and diff UART.

**Tech Stack:** C11, CMake 3.20+, Ninja, arm-none-eabi-gcc, qemu-system-arm, gdb-multiarch, FreeRTOS-Kernel V11.1.0, CMSIS-Core (no STM32Cube HAL).

## Global Constraints

- QEMU machine is `netduinoplus2` (STM32F405, Cortex-M4F). No Cube HAL (`HAL_UART_*`, `HAL_Init`).
- Labs use FreeRTOS API + BSP: `uart_puts`, `uart_printf`, `bsp_irq_init`, `bsp_irq_pend`, `BSP_IRQHandler`. SysTick is FreeRTOS-owned.
- `configASSERT` on; malloc-failed hook prints `HOOK MALLOC_FAILED`; stack overflow hook prints `HOOK STACK_OVERFLOW`; then hang.
- Every firmware prints a last UART line `DONE`. Phase 1 has `expected.log`; Phase 2–3 do not use `check`.
- Lab folders: `README.md` (Why / What to memorize / Your job / How to run / Done when / GDB if Phase 2–3), `FreeRTOSConfig.h`, `src/app.c`, `solution/app.c`, `solution/NOTES.md`.
- Run UX: `./scripts/install-env.sh` then `./scripts/lab.sh qemu 01` (not Make).
- Kernel: one vendored tree `third_party/FreeRTOS-Kernel` at tag `V11.1.0`. Port `GCC/ARM_CM4F`, `heap_4`.
- Out of v1: MPU, tickless, +TCP/+FAT, Docker, CMSIS-RTOS, host unit tests.

## File map

| Path | Responsibility |
|------|----------------|
| `scripts/install-env.sh` | apt + verify cmake, ninja, arm-none-eabi-gcc, qemu-system-arm, gdb-multiarch |
| `scripts/lab.sh` | `qemu\|solution\|check\|gdb <NN>` |
| `scripts/run-qemu.sh` | invoke QEMU on an ELF |
| `scripts/check-uart.sh` | timeout, capture until `DONE`, diff expected.log |
| `cmake/gcc-arm-none-eabi.cmake` | toolchain |
| `cmake/FreeRTOS.cmake` | kernel sources + include paths |
| `cmake/AddLab.cmake` | `add_freertos_lab(NN slug)` |
| `CMakeLists.txt` | project, add all labs |
| `bsp/startup.s` | Cortex-M4 vectors; weak `BSP_IRQHandler` aliased to EXTI0 |
| `bsp/linker.ld` | FLASH/RAM for STM32F405 |
| `bsp/system_stm32f4xx.c` | empty `SystemInit` |
| `bsp/include/stm32f405xx.h` | minimal IRQn, USART2, RCC |
| `bsp/uart.c` `bsp/uart.h` | polled USART2 |
| `bsp/irq.c` `bsp/irq.h` | `bsp_irq_init`, `bsp_irq_pend` on `EXTI0_IRQn` |
| `bsp/hooks.c` | assert / malloc / stack overflow |
| `bsp/include/FreeRTOSConfig.default.h` | default config |
| `third_party/cmsis/` | CMSIS-Core Include only |
| `third_party/FreeRTOS-Kernel/` | submodule or snapshot V11.1.0 |
| `labs/NN-slug/` | one directory per lab |

---

### Task 1: Host env script and gitignore

**Files:**
- Create: `.gitignore`
- Create: `scripts/install-env.sh`
- Create: `README.md` (bootstrap only; full lab index in Task 15)

**Interfaces:**
- Consumes: Ubuntu/WSL2 `apt-get`
- Produces: exit 0 iff `cmake`, `ninja`, `arm-none-eabi-gcc`, `qemu-system-arm`, and (`gdb-multiarch` or `arm-none-eabi-gdb`) are on `PATH`

- [ ] **Step 1: Write `.gitignore`**

```
build/
.cache/
*.elf
*.bin
*.map
*.o
```

- [ ] **Step 2: Write `scripts/install-env.sh`**

```bash
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
echo "env OK"
```

- [ ] **Step 3: `chmod +x scripts/install-env.sh` and run it**

Run: `./scripts/install-env.sh`
Expected: last line `env OK`; all five tools present. If apt fails, stop and fix packages before any firmware work.

- [ ] **Step 4: Write a short `README.md`**

Title, one paragraph goal, then:

```markdown
## Setup
./scripts/install-env.sh
./scripts/lab.sh qemu 01
```

(`lab.sh` will appear in Task 5; this sentence is the contract.)

- [ ] **Step 5: Commit**

```bash
git add .gitignore scripts/install-env.sh README.md
git commit -m "Add WSL env installer and gitignore."
```

---

### Task 2: CMake toolchain, startup, UART hello (no FreeRTOS)

**Files:**
- Create: `cmake/gcc-arm-none-eabi.cmake`
- Create: `bsp/linker.ld`
- Create: `bsp/startup.s`
- Create: `bsp/include/stm32f405xx.h`
- Create: `bsp/system_stm32f4xx.c`
- Create: `bsp/uart.h` `bsp/uart.c`
- Create: `bsp/CMakeLists.txt`
- Create: `CMakeLists.txt`
- Create: `labs/_uart_smoke/main.c` (temporary; deleted in Task 6)

**Interfaces:**
- Consumes: `arm-none-eabi-gcc`, QEMU `netduinoplus2`
- Produces: `uart_puts(const char *)`, `uart_printf(const char *fmt, ...)`, USART2 at 115200 assuming QEMU default clocks after enabling RCC

- [ ] **Step 1: Write the toolchain file `cmake/gcc-arm-none-eabi.cmake`**

```cmake
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_SIZE arm-none-eabi-size)
set(CMAKE_C_FLAGS_INIT "-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -ffunction-sections -fdata-sections")
set(CMAKE_ASM_FLAGS_INIT "-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16")
set(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,--gc-sections -specs=nano.specs -specs=nosys.specs")
```

- [ ] **Step 2: Write `bsp/linker.ld`**

```ld
MEMORY {
  FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 1024K
  RAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 128K
}
ENTRY(Reset_Handler)
SECTIONS {
  .isr_vector : { KEEP(*(.isr_vector)) } > FLASH
  .text : { *(.text*) *(.rodata*) } > FLASH
  _sidata = LOADADDR(.data);
  .data : { _sdata = .; *(.data*); _edata = .; } > RAM AT > FLASH
  .bss (NOLOAD) : { _sbss = .; *(.bss*) *(COMMON); _ebss = .; } > RAM
  ._heap (NOLOAD) : { . = ALIGN(8); _heap_start = .; . += 48K; _heap_end = .; } > RAM
  ._stack (NOLOAD) : { . = ALIGN(8); . = ORIGIN(RAM) + LENGTH(RAM); _estack = .; } > RAM
}
```

- [ ] **Step 3: Write minimal `bsp/include/stm32f405xx.h`**

Include: `__IO` via `stdint.h`; `EXTI0_IRQn = 6`; `USART2_IRQn = 38`; `USART2_BASE 0x40004400`; `RCC_BASE 0x40023800`; `RCC_APB1ENR` bit 17 USART2EN; USART `SR`, `DR`, `BRR`, `CR1` with `UE`, `TE`, `RE`, `TXE`. Enough for polled TX and NVIC pending EXTI0. Do not copy Cube HAL.

- [ ] **Step 4: Write `bsp/startup.s`**

Vector table at `.isr_vector`: `_estack`, `Reset_Handler`, NMI/HardFault/MemManage/BusFault/UsageFault, 0 padding, SVC, DebugMon, 0, PendSV, SysTick, then IRQs. Position 6 (EXTI0) is `BSP_IRQHandler`. Provide weak `Default_Handler` infinite loop. `BSP_IRQHandler` weak, aliases `Default_Handler`. `Reset_Handler`: copy `.data`, zero `.bss`, `bl SystemInit`, `bl main`, then loop.

- [ ] **Step 5: Write UART**

`uart.h`:

```c
#pragma once
void uart_init(void);
void uart_puts(const char *s);
void uart_printf(const char *fmt, ...);
```

`uart.c`: enable USART2 in `RCC->APB1ENR`, set `BRR` for 115200 at 16 MHz (QEMU netduinoplus2 default HSI), `CR1 = UE|TE|RE`. `uart_puts` waits `TXE`. `uart_printf` uses `vsnprintf` into a 128-byte stack buffer then `uart_puts`.

`system_stm32f4xx.c`: `void SystemInit(void) {}`

- [ ] **Step 6: Write CMake to build `uart_smoke`**

Root `CMakeLists.txt`: `cmake_minimum_required(VERSION 3.20)`, `project(freertos_labs C ASM)`, `add_subdirectory(bsp)`, temporary executable `uart_smoke` from `labs/_uart_smoke/main.c` linking `bsp`.

`labs/_uart_smoke/main.c`: `uart_init(); uart_puts("HELLO\r\nDONE\r\n"); for(;;);`

- [ ] **Step 7: Configure, build, run QEMU (this is the test)**

```bash
cmake -B build -G Ninja -DCMAKE_TOOLCHAIN_FILE=cmake/gcc-arm-none-eabi.cmake
cmake --build build --target uart_smoke
qemu-system-arm -M netduinoplus2 -nographic -serial stdio -kernel build/uart_smoke.elf
```

Expected: serial contains `HELLO` and `DONE`. If UART is silent, adjust `BRR` / USART instance (try USART1) until QEMU prints; do not proceed with a dead console.

- [ ] **Step 8: Commit**

```bash
git add cmake/gcc-arm-none-eabi.cmake CMakeLists.txt bsp labs/_uart_smoke
git commit -m "Boot QEMU STM32F4 with polled UART."
```

---

### Task 3: CMSIS-Core, IRQ BSP, hooks stubs

**Files:**
- Create: `third_party/cmsis/README.md` (source URL + version)
- Create: `bsp/irq.h` `bsp/irq.c`
- Create: `bsp/hooks.c` (assert hang only until FreeRTOS exists)
- Modify: `bsp/CMakeLists.txt` to add cmsis include and irq.c
- Modify: `startup.s` if EXTI0 slot was wrong

**Interfaces:**
- Consumes: CMSIS `core_cm4.h`, `EXTI0_IRQn`
- Produces:

```c
void bsp_irq_init(uint32_t nvic_priority); /* 0=highest */
void bsp_irq_pend(void);
void BSP_IRQHandler(void); /* lab overrides; weak default empty-return after clearing */
```

- [ ] **Step 1: Vendor CMSIS-Core**

Copy ARM CMSIS-Core `Include/` (`core_cm4.h`, `cmsis_compiler.h`, `cmsis_gcc.h`, `cmsis_version.h`, `mpu_armv7.h` if required by core_cm4) into `third_party/cmsis/Include/`. Record tag in `third_party/cmsis/README.md` (CMSIS_5 5.9.0 or CMSIS_6 equivalent). Do not add STM32Cube.

- [ ] **Step 2: Implement `bsp/irq.h` / `irq.c`**

```c
#pragma once
#include <stdint.h>
void bsp_irq_init(uint32_t nvic_priority);
void bsp_irq_pend(void);
void BSP_IRQHandler(void);
```

`irq.c`: `#include "stm32f405xx.h"` and `core_cm4.h`. `bsp_irq_init`: `NVIC_SetPriority(EXTI0_IRQn, nvic_priority); NVIC_EnableIRQ(EXTI0_IRQn);`. `bsp_irq_pend`: `NVIC_SetPendingIRQ(EXTI0_IRQn);`. Strong `BSP_IRQHandler` in irq.c as weak symbol using `__attribute__((weak))` that returns. Labs provide a strong `BSP_IRQHandler`.

- [ ] **Step 3: Smoke: pend IRQ from main, handler prints `IRQ` then `DONE`**

Temporarily in `_uart_smoke/main.c`: implement `BSP_IRQHandler` printing `IRQ\r\n`, `bsp_irq_init(5); bsp_irq_pend();` then loop. Rebuild and QEMU.

Expected: `IRQ` then you still need a `DONE` from main after a short spin (handler ran). If IRQ never fires, fix vector index for EXTI0 on STM32F4 (IRQ0 WWDG … IRQ6 EXTI0).

- [ ] **Step 4: Commit**

```bash
git add third_party/cmsis bsp/irq.c bsp/irq.h bsp/CMakeLists.txt labs/_uart_smoke
git commit -m "Add CMSIS-Core and dedicated EXTI0 BSP IRQ."
```

---

### Task 4: Vendor FreeRTOS-Kernel and default config

**Files:**
- Create: `third_party/FreeRTOS-Kernel/` (git submodule tag `V11.1.0` **or** snapshot of that tag)
- Create: `cmake/FreeRTOS.cmake`
- Create: `bsp/include/FreeRTOSConfig.default.h`
- Create: `bsp/hooks.c` (full)
- Modify: `CMakeLists.txt` / `bsp/CMakeLists.txt`

**Interfaces:**
- Consumes: Kernel `tasks.c`, `queue.c`, `list.c`, `timers.c`, `event_groups.c`, `stream_buffer.c`, `portable/GCC/ARM_CM4F/port.c`, `portable/MemMang/heap_4.c`
- Produces: CMake interface library `freertos_kernel`; labs must compile with `-I` to their own `FreeRTOSConfig.h`

- [ ] **Step 1: Add kernel at V11.1.0**

```bash
git submodule add https://github.com/FreeRTOS/FreeRTOS-Kernel.git third_party/FreeRTOS-Kernel
cd third_party/FreeRTOS-Kernel && git checkout V11.1.0
```

If submodule is unavailable, download the `V11.1.0` source tarball into that path and commit it.

- [ ] **Step 2: Write `bsp/include/FreeRTOSConfig.default.h`**

Required knobs (exact values):

```c
#define configCPU_CLOCK_HZ                         16000000
#define configTICK_RATE_HZ                         1000
#define configMAX_PRIORITIES                       7
#define configMINIMAL_STACK_SIZE                   128
#define configTOTAL_HEAP_SIZE                      (48 * 1024)
#define configSUPPORT_STATIC_ALLOCATION            1
#define configSUPPORT_DYNAMIC_ALLOCATION           1
#define configMAX_TASK_NAME_LEN                    16
#define configUSE_PREEMPTION                       1
#define configUSE_TIME_SLICING                     1
#define configUSE_MUTEXES                          1
#define configUSE_RECURSIVE_MUTEXES                1
#define configUSE_COUNTING_SEMAPHORES              1
#define configUSE_TASK_NOTIFICATIONS               1
#define configUSE_QUEUE_SETS                       0
#define configUSE_TIMERS                           1
#define configTIMER_TASK_PRIORITY                  2
#define configTIMER_QUEUE_LENGTH                   8
#define configTIMER_TASK_STACK_DEPTH               256
#define configUSE_IDLE_HOOK                        0
#define configUSE_TICK_HOOK                        0
#define configUSE_MALLOC_FAILED_HOOK               1
#define configCHECK_FOR_STACK_OVERFLOW             2
#define configUSE_16_BIT_TICKS                     0
#define configIDLE_SHOULD_YIELD                    1
#define configENABLE_BACKWARD_COMPATIBILITY        0
#define INCLUDE_vTaskDelay                         1
#define INCLUDE_vTaskDelayUntil                    1
#define INCLUDE_vTaskDelete                        1
#define INCLUDE_vTaskSuspend                       1
#define INCLUDE_xTaskGetCurrentTaskHandle          1
#define INCLUDE_uxTaskPriorityGet                  1
#define INCLUDE_uxTaskGetStackHighWaterMark        1
#define configPRIO_BITS                            4
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY    15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5
#define configKERNEL_INTERRUPT_PRIORITY            (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))
#define configMAX_SYSCALL_INTERRUPT_PRIORITY       (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))
#define configASSERT(x) if ((x) == 0) { taskDISABLE_INTERRUPTS(); for(;;) {} }
```

Also enable stream/message buffers (`configUSE_STREAM_BUFFERS` / `configUSE_MESSAGE_BUFFERS` if V11 uses those names; otherwise the files are compiled and APIs exist by default — match V11.1.0 headers).

- [ ] **Step 3: `bsp/hooks.c`**

```c
void vApplicationMallocFailedHook(void) {
  uart_puts("HOOK MALLOC_FAILED\r\n");
  for (;;) {}
}
void vApplicationStackOverflowHook(TaskHandle_t t, char *n) {
  (void)t; (void)n;
  uart_puts("HOOK STACK_OVERFLOW\r\n");
  for (;;) {}
}
void vApplicationGetIdleTaskMemory(StaticTask_t **tcb, StackType_t **stack, uint32_t *size) {
  static StaticTask_t idle_tcb;
  static StackType_t idle_stack[configMINIMAL_STACK_SIZE];
  *tcb = &idle_tcb;
  *stack = idle_stack;
  *size = configMINIMAL_STACK_SIZE;
}
void vApplicationGetTimerTaskMemory(StaticTask_t **tcb, StackType_t **stack, uint32_t *size) {
  static StaticTask_t tmr_tcb;
  static StackType_t tmr_stack[configTIMER_TASK_STACK_DEPTH];
  *tcb = &tmr_tcb;
  *stack = tmr_stack;
  *size = configTIMER_TASK_STACK_DEPTH;
}
```

Default config sets `configSUPPORT_STATIC_ALLOCATION 1` and `configSUPPORT_DYNAMIC_ALLOCATION 1`.

- [ ] **Step 4: `cmake/FreeRTOS.cmake`**

`add_library(freertos_kernel STATIC ...)` with the kernel `.c` files listed above. `target_include_directories` PUBLIC: kernel `include`, `portable/GCC/ARM_CM4F`. Do **not** put `FreeRTOSConfig.h` on this library; each lab target adds its lab directory as PRIVATE include.

- [ ] **Step 5: Commit**

```bash
git add third_party/FreeRTOS-Kernel cmake/FreeRTOS.cmake bsp/hooks.c bsp/include/FreeRTOSConfig.default.h
git commit -m "Vendor FreeRTOS-Kernel V11.1.0 and default config."
```

---

### Task 5: `add_freertos_lab`, `lab.sh`, QEMU helpers

**Files:**
- Create: `cmake/AddLab.cmake`
- Create: `scripts/run-qemu.sh`
- Create: `scripts/check-uart.sh`
- Create: `scripts/lab.sh`
- Modify: `CMakeLists.txt`

**Interfaces:**
- Consumes: lab id `01`, slug folder `labs/01-tasks`
- Produces: targets `lab_01`, `lab_01_solution`, `qemu-01`, `qemu-01-solution`, `check-01`; CLI `./scripts/lab.sh qemu 01`

- [ ] **Step 1: Write `cmake/AddLab.cmake`**

```cmake
function(add_freertos_lab ID SLUG)
  set(LAB_DIR ${CMAKE_SOURCE_DIR}/labs/${ID}-${SLUG})
  add_executable(lab_${ID} ${LAB_DIR}/src/app.c)
  add_executable(lab_${ID}_solution ${LAB_DIR}/solution/app.c)
  foreach(tgt lab_${ID} lab_${ID}_solution)
    target_include_directories(${tgt} PRIVATE ${LAB_DIR} ${CMAKE_SOURCE_DIR}/bsp/include)
    target_link_libraries(${tgt} PRIVATE bsp freertos_kernel)
    target_link_options(${tgt} PRIVATE -T${CMAKE_SOURCE_DIR}/bsp/linker.ld "-Wl,-Map=$<TARGET_FILE_DIR:${tgt}>/${tgt}.map")
  endforeach()
  add_custom_target(qemu-${ID}
    COMMAND ${CMAKE_SOURCE_DIR}/scripts/run-qemu.sh $<TARGET_FILE:lab_${ID}>
    DEPENDS lab_${ID} USES_TERMINAL)
  add_custom_target(qemu-${ID}-solution
    COMMAND ${CMAKE_SOURCE_DIR}/scripts/run-qemu.sh $<TARGET_FILE:lab_${ID}_solution>
    DEPENDS lab_${ID}_solution USES_TERMINAL)
  if(EXISTS ${LAB_DIR}/expected.log)
    add_custom_target(check-${ID}
      COMMAND ${CMAKE_SOURCE_DIR}/scripts/check-uart.sh
              $<TARGET_FILE:lab_${ID}_solution> ${LAB_DIR}/expected.log
      DEPENDS lab_${ID}_solution)
  endif()
endfunction()
```

- [ ] **Step 2: `scripts/run-qemu.sh`**

```bash
#!/usr/bin/env bash
set -euo pipefail
ELF=$1
exec qemu-system-arm -M netduinoplus2 -nographic -serial stdio -kernel "$ELF"
```

- [ ] **Step 3: `scripts/check-uart.sh`**

```bash
#!/usr/bin/env bash
set -euo pipefail
ELF=$1
EXPECT=$2
OUT=$(mktemp)
TIMEOUT=8
# timeout(1) sends SIGTERM; QEMU should die. Capture stdout.
set +e
timeout --signal=KILL ${TIMEOUT}s qemu-system-arm -M netduinoplus2 -nographic -serial stdio -kernel "$ELF" >"$OUT" 2>&1
set -e
if ! grep -q '^DONE$' "$OUT"; then
  echo "timeout or missing DONE" >&2
  cat "$OUT" >&2
  exit 1
fi
# Keep lines through first DONE inclusive for diff
awk '1; /^DONE$/{exit}' "$OUT" | tr -d '\r' > "${OUT}.norm"
tr -d '\r' < "$EXPECT" > "${OUT}.exp"
diff -u "${OUT}.exp" "${OUT}.norm"
```

`expected.log` uses `\n` lines ending with `DONE` and no extra QEMU banners. If QEMU prints a banner, strip non-lab lines in `check-uart.sh` with `grep -v` for `QEMU` or pass `-nodefaults` as needed so the diff is stable.

- [ ] **Step 4: `scripts/lab.sh`**

```bash
#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "$0")/.." && pwd)
CMD=$1
ID=$(printf '%02d' "$2")
if [[ ! -d "$ROOT/build" ]]; then
  cmake -B "$ROOT/build" -G Ninja -DCMAKE_TOOLCHAIN_FILE="$ROOT/cmake/gcc-arm-none-eabi.cmake" -S "$ROOT"
fi
case $CMD in
  qemu)     cmake --build "$ROOT/build" --target qemu-${ID} ;;
  solution) cmake --build "$ROOT/build" --target qemu-${ID}-solution ;;
  check)    cmake --build "$ROOT/build" --target check-${ID} ;;
  gdb)
    cmake --build "$ROOT/build" --target lab_${ID}
    ELF=$(find "$ROOT/build" -name "lab_${ID}" -o -name "lab_${ID}.elf" | head -1)
    qemu-system-arm -M netduinoplus2 -nographic -serial stdio -s -S -kernel "$ELF" &
    QPID=$!
    trap 'kill $QPID 2>/dev/null || true' EXIT
    GDB=$(command -v gdb-multiarch || command -v arm-none-eabi-gdb)
    "$GDB" -ex "target remote :1234" "$ELF"
    ;;
  *) echo "usage: $0 qemu|solution|check|gdb NN" >&2; exit 1 ;;
esac
```

chmod +x the three scripts.

- [ ] **Step 5: Commit**

```bash
git add cmake/AddLab.cmake scripts CMakeLists.txt
git commit -m "Add lab CMake helper and QEMU run/check scripts."
```

---

### Task 6: Lab 01 tasks (first real lab; delete uart_smoke)

**Files:**
- Create: `labs/01-tasks/FreeRTOSConfig.h` (include default or copy)
- Create: `labs/01-tasks/src/app.c`
- Create: `labs/01-tasks/solution/app.c`
- Create: `labs/01-tasks/solution/NOTES.md`
- Create: `labs/01-tasks/README.md`
- Create: `labs/01-tasks/expected.log`
- Modify: `CMakeLists.txt` — `include(cmake/AddLab.cmake)` `add_freertos_lab(01 tasks)`; remove `uart_smoke`
- Delete: `labs/_uart_smoke/`

**Interfaces:**
- Consumes: `xTaskCreate`, `vTaskStartScheduler`, `vTaskDelay`, `vTaskSuspend`, `vTaskResume`, UART
- Produces: UART sequence in `expected.log`; target `lab_01`

Each lab `FreeRTOSConfig.h`:

```c
#pragma once
#include "FreeRTOSConfig.default.h"
```

unless that lab must override a knob.

- [ ] **Step 1: Write `expected.log` first (the test)**

```
low
high
resumed
DONE
```

- [ ] **Step 2: Write `solution/app.c` (must satisfy the log)**

Create two tasks: `high` priority 2 prints `high` then `vTaskDelay(10)`; `low` priority 1 prints `low`, creates/resumes a third `paused` task that was created suspended (`vTaskSuspend` in creator or `xTaskCreate` then `vTaskSuspend`), then `vTaskResume`. After `resumed` printed by the paused task, a monitor task or the paused task prints `DONE` and `vTaskSuspend(NULL)` so the log is stable. Use `uart_init()` before `vTaskStartScheduler`. Stack size `configMINIMAL_STACK_SIZE * 2`.

Exact control flow for the golden log:

1. Start scheduler.
2. Low runs first if high is delayed… **Do not rely on startup order.** Instead: high waits on a notification; low prints `low`, gives notification; high prints `high`; low resumes `paused`; paused prints `resumed` then `DONE`.

Simpler deterministic sequence (use this):

- `main` creates `sequencer` priority 3 only.
- Sequencer: create `worker` suspended, print nothing, `uart_puts("low\r\n")`, `uart_puts("high\r\n")`, `vTaskResume(worker)`, worker prints `resumed`, sequencer prints `DONE`.

That is too fake. **Required APIs must actually run:**

```c
static TaskHandle_t high_h, paused_h;

static void paused(void *a) {
  (void)a;
  uart_puts("resumed\r\nDONE\r\n");
  for (;;) vTaskDelay(1000);
}

static void high_task(void *a) {
  (void)a;
  ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
  uart_puts("high\r\n");
  vTaskResume(paused_h);
  for (;;) vTaskDelay(1000);
}

static void low_task(void *a) {
  (void)a;
  uart_puts("low\r\n");
  xTaskNotifyGive(high_h);
  for (;;) vTaskDelay(1000);
}

int main(void) {
  uart_init();
  xTaskCreate(paused, "p", 256, NULL, 1, &paused_h);
  vTaskSuspend(paused_h);
  xTaskCreate(high_task, "h", 256, NULL, 3, &high_h);
  xTaskCreate(low_task, "l", 256, NULL, 2, NULL);
  vTaskStartScheduler();
  for (;;) {}
}
```

Lab 01 README must still teach notifications only as “used here to order prints”; the memorize list is create/delay/suspend/resume/delete/states. Add `vTaskDelete` on a dummy task created and deleted in `main` before scheduler start so the delete API is used:

```c
TaskHandle_t tmp;
xTaskCreate(paused, "tmp", 256, NULL, 1, &tmp);
vTaskDelete(tmp);
```

Do not use the same function if that unblocks UART; use an empty `dummy` task function.

- [ ] **Step 3: Write `src/app.c` with TODOs**

Same structure; replace bodies of create/suspend/resume/notify with `/* TODO */` so it compiles but will not match `expected.log` (empty `main` that prints nothing except maybe compile-time stubs calling `vTaskStartScheduler` without tasks is OK). Prefer compiling stubs:

```c
int main(void) {
  uart_init();
  /* TODO: xTaskCreate high, low, paused; vTaskSuspend paused; vTaskDelete dummy */
  vTaskStartScheduler();
  for (;;) {}
}
```

- [ ] **Step 4: README + NOTES**

README headings in order. How to run: `./scripts/lab.sh qemu 01` and `./scripts/lab.sh check 01`. Memorize: `xTaskCreate` stack in **words**, priorities, states, `vTaskDelay`, suspend/resume/delete, idle+timer need heap at start.

NOTES: why notify was used to make UART deterministic; stack units; if scheduler returns, heap was too small.

- [ ] **Step 5: Wire CMake, run check**

```bash
./scripts/lab.sh check 01
```

Expected: `diff` succeeds (uses **solution** ELF per spec). Then `./scripts/lab.sh qemu 01` on learner src does **not** need to pass check.

- [ ] **Step 6: Commit**

```bash
git add CMakeLists.txt labs/01-tasks
git rm -r labs/_uart_smoke
git commit -m "Add lab 01 tasks with UART golden check."
```

---

### Task 7: Labs 02–04 (timing, queues, semaphores)

**Files:** create the full lab contract for `02-timing`, `03-queues`, `04-semaphores`. `add_freertos_lab` for each in `CMakeLists.txt`.

**Interfaces:** same BSP + kernel. Phase 1 `expected.log` required.

- [ ] **Step 1: Lab 02 timing**

`expected.log`:

```
a
b
a
b
DONE
```

Solution: two tasks using `vTaskDelayUntil` with 50 ms period, staggered by 25 ms, print 2 cycles then `DONE` from a third task that `vTaskDelay` 120 ms. `src/app.c` uses `vTaskDelay` TODOs. README memorize: `vTaskDelay` vs `Until`, `pdMS_TO_TICKS`, tick 1000 Hz.

- [ ] **Step 2: Lab 03 queues**

`expected.log`:

```
42
DONE
```

Solution: producer `xQueueSend` uint32 `42`, consumer `xQueueReceive` prints it. Queue length 1, item size `sizeof(uint32_t)`. `src`: TODOs on create/send/recv. Memorize: copy vs reference, block time, length × item size.

- [ ] **Step 3: Lab 04 semaphores**

`expected.log`:

```
took
took
DONE
```

Solution: counting semaphore max 2, give twice in `main` before start (or giver task), two takes print `took`. Memorize: binary vs counting.

- [ ] **Step 4: Run checks**

```bash
./scripts/lab.sh check 02
./scripts/lab.sh check 03
./scripts/lab.sh check 04
```

Expected: all three diffs pass on solution builds.

- [ ] **Step 5: Commit**

```bash
git add CMakeLists.txt labs/02-timing labs/03-queues labs/04-semaphores
git commit -m "Add labs 02-04 timing, queues, and semaphores."
```

---

### Task 8: Labs 05–08 (mutex, notifications, event groups, timers)

**Files:** `labs/05-mutex` … `labs/08-timers` + CMake entries.

- [ ] **Step 1: Lab 05 mutex**

`expected.log`: `locked` / `unlocked` / `DONE`. Two tasks take the same mutex; prints serialize. README: mutex vs binary semaphore, **not FromISR**, recursive mutex one paragraph. `src` TODOs.

- [ ] **Step 2: Lab 06 notifications**

`expected.log`: `n1` / `DONE`. Task A `xTaskNotifyGive`, B `ulTaskNotifyTake`. Memorize: notify as binary/counting/mailbox vs queue.

- [ ] **Step 3: Lab 07 event groups**

`expected.log`: `sync` / `DONE`. Two tasks `xEventGroupSync` bits 1 and 2. Memorize: wait-all vs wait-any.

- [ ] **Step 4: Lab 08 timers**

`expected.log`: `tick` / `tick` / `DONE`. Auto-reload timer 50 ms, callback counts 2 then stops timer and prints DONE (callback must not block; `xTimerStop` from callback is allowed). Memorize: daemon task, no blocking in callback.

- [ ] **Step 5: `./scripts/lab.sh check 05` … `08` — all pass**

- [ ] **Step 6: Commit**

```bash
git add CMakeLists.txt labs/05-mutex labs/06-notifications labs/07-event-groups labs/08-timers
git commit -m "Add labs 05-08 mutex, notify, events, timers."
```

---

### Task 9: Labs 09–12 (buffers, ISR APIs, static alloc, heap)

**Files:** `labs/09-stream-message-buffers` `labs/10-isr-apis` `labs/11-static-alloc` `labs/12-heap`

- [ ] **Step 1: Lab 09**

One writer task, one reader task, `xStreamBufferSend/Receive`. `expected.log`: `abc` / `DONE`. README **What to memorize** includes **single writer and single reader**; NOTES explain why two writers are unsafe. `src` TODOs.

- [ ] **Step 2: Lab 10 ISR APIs**

`expected.log`: `isr` / `DONE`.

`main`: `uart_init(); bsp_irq_init(5);` create receiver task blocked on queue; pender task loops `bsp_irq_pend()`.

```c
void BSP_IRQHandler(void) {
  BaseType_t woken = pdFALSE;
  uint32_t m = 1;
  xQueueSendFromISR(q, &m, &woken);
  portYIELD_FROM_ISR(woken);
}
```

Receiver prints `isr` then `DONE`. `src` uses `xQueueSend` (wrong) in the handler as TODO to replace with FromISR. (If wrong API hits assert, learner still has a compiling file — Phase 1 should use TODOs that compile: empty handler plus comments.)

Learner `src`: empty `BSP_IRQHandler` and TODOs; solution has FromISR.

- [ ] **Step 3: Lab 11 static alloc**

`expected.log`: `static` / `DONE`. `xTaskCreateStatic` with `StaticTask_t` + stack array. Override config: `configSUPPORT_STATIC_ALLOCATION 1`. Memorize NULL buffers.

- [ ] **Step 4: Lab 12 heap**

Do **not** hit the hook on the solution path. `expected.log`: `alloc_ok` / `DONE`. Solution `pvPortMalloc` 16 bytes, print `alloc_ok`, `vPortFree`. README documents `HOOK MALLOC_FAILED` and that `vTaskStartScheduler` needs heap for idle/timer. `src` TODO malloc.

- [ ] **Step 5: Run `check 09`–`12`**

- [ ] **Step 6: Commit**

```bash
git add CMakeLists.txt labs/09-stream-message-buffers labs/10-isr-apis labs/11-static-alloc labs/12-heap
git commit -m "Add labs 09-12 buffers, ISR, static alloc, heap."
```

---

### Task 10: Labs 13–16 (assert, wrong ISR API, NVIC priority, stack overflow)

Phase 2: **no** `expected.log`. `src` compiles and misbehaves; `solution` fixes. README **GDB** section with breakpoints. `check-` targets are not created.

**Interfaces:** same `add_freertos_lab` without expected.log.

- [ ] **Step 1: Lab 13 configassert**

`src`: `xQueueReceive(NULL, ...)` or give mutex from ISR-like misuse in task: `xQueueCreate(0, 0)` then assert. Hang in `configASSERT`. Solution: valid queue, prints `alive` `DONE`. GDB: break on `configASSERT` expansion loop in `FreeRTOSConfig.h`. Done when: UART shows `DONE` instead of hang.

- [ ] **Step 2: Lab 14 isr-wrong-api**

`src`: `BSP_IRQHandler` calls `xQueueSend` (task API). Solution: `xQueueSendFromISR` + yield. GDB: break `BSP_IRQHandler`. Symptom: assert hang or never unblocks. Pender task as lab 10.

- [ ] **Step 3: Lab 15 nvic-syscall-priority**

`src`: `bsp_irq_init(0)` (priority too high / numerically smaller than syscall max) then `FromISR`. Solution: `bsp_irq_init(5)` matching `configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY`. README explains BASEPRI. GDB: inspect NVIC priority registers.

- [ ] **Step 4: Lab 16 stack overflow**

`src`: task with `configMINIMAL_STACK_SIZE` that recurses or `uint8_t buf[2048]` on stack. Must print `HOOK STACK_OVERFLOW` then hang. Solution: larger stack, prints `DONE`. `FreeRTOSConfig.h` keeps `configCHECK_FOR_STACK_OVERFLOW 2`.

- [ ] **Step 5: Manual verify**

```bash
./scripts/lab.sh qemu 13   # hang (src)
./scripts/lab.sh solution 13  # DONE
./scripts/lab.sh solution 16  # DONE
./scripts/lab.sh qemu 16      # HOOK STACK_OVERFLOW
```

- [ ] **Step 6: Commit**

```bash
git add CMakeLists.txt labs/13-configassert labs/14-isr-wrong-api labs/15-nvic-syscall-priority labs/16-stack-overflow
git commit -m "Add Phase 2 labs 13-16 assert, ISR, NVIC, stack."
```

---

### Task 11: Labs 17–20 (inversion, deadlock, starvation, heap leak)

- [ ] **Step 1: Lab 17 priority-inversion**

`src`: high and low share a **binary semaphore** as lock; medium hogs CPU; high blocked long. UART never reaches `DONE` in a 3 s window (or prints `late`). Solution: mutex so inheritance lets high run; prints `DONE`. README: inversion vs inheritance.

- [ ] **Step 2: Lab 18 deadlock**

`src`: task A takes m1 then m2; B takes m2 then m1; both delay between takes. Hang. Solution: same lock order. Prints `DONE`.

- [ ] **Step 3: Lab 19 starvation**

`src`: `configUSE_TIME_SLICING 0` overlay, two equal-priority tasks, one never prints. Solution: `configUSE_TIME_SLICING 1` or different priorities + delay. `DONE` when both printed.

- [ ] **Step 4: Lab 20 heap-leak**

`src`: loop `xQueueCreate` without `vQueueDelete`; after N iterations `HOOK MALLOC_FAILED`. Solution: delete queues / reuse one queue; print `DONE`.

- [ ] **Step 5: Verify solution ELFs print `DONE`; src shows the documented symptom**

- [ ] **Step 6: Commit**

```bash
git add CMakeLists.txt labs/17-priority-inversion labs/18-deadlock labs/19-starvation labs/20-heap-leak
git commit -m "Add Phase 2 labs 17-20 inversion, deadlock, starvation, leak."
```

---

### Task 12: Labs 21–23 (timer block, race, critical vs mutex)

- [ ] **Step 1: Lab 21 timer-callback-blocks**

`src`: auto-reload timer callback `vTaskDelay(100)` — daemon stalls; second timer never fires. Solution: callback only `xTaskNotifyGive` a worker. `DONE` from worker.

- [ ] **Step 2: Lab 22 data-race**

`src`: two tasks `counter++` without protection; print counter (wrong). Solution: mutex around increment; print exact `20000` then `DONE`. (No `expected.log` in Phase 2; README says done when count is 20000.)

- [ ] **Step 3: Lab 23 critical-vs-mutex**

Three short demos in one `app.c` (solution): (1) `taskENTER_CRITICAL` around `counter++`; (2) `vTaskSuspendAll`/`xTaskResumeAll`; (3) mutex. README memorize table: ISR-safe? can block? duration. `src`: all three TODOs empty so counter is wrong. `DONE` after solution prints `ok`.

- [ ] **Step 4: Verify solutions emit `DONE`**

- [ ] **Step 5: Commit**

```bash
git add CMakeLists.txt labs/21-timer-callback-blocks labs/22-data-race labs/23-critical-vs-mutex
git commit -m "Add Phase 2 labs 21-23 timer, race, critical sections."
```

---

### Task 13: Labs 24–28 (internals)

Phase 3: READMEs point at **exact kernel files** in `third_party/FreeRTOS-Kernel/`. Learner `src` is a small annotation or config change; solution is the working config/demo.

- [ ] **Step 1: Lab 24 freertos-config**

README walks every knob in `FreeRTOSConfig.default.h`. `src` sets `configTICK_RATE_HZ 100` by mistake; UART timing demo looks wrong. Solution restores 1000. GDB: `xTickCount`. `DONE` after 10 task delays of 10 ms wall-ish (QEMU).

- [ ] **Step 2: Lab 25 tcb-ready-lists**

README: `TCB_t` in `tasks.c`, `pxReadyTasksLists`. `src`/`solution`: three tasks different priorities; print run order `3` `2` `1` `DONE`. GDB: break `vTaskStartScheduler`, inspect ready lists.

- [ ] **Step 3: Lab 26 systick-tick**

README: `xPortSysTickHandler` in `port.c`, `xTaskIncrementTick`. Solution: idle-priority task prints `tick` using `xTaskGetTickCount` after `vTaskDelay(5)` then `DONE`. GDB: `SysTick_Handler`.

- [ ] **Step 4: Lab 27 context-switch**

README: `PendSV_Handler` / `SVC_Handler` in `portable/GCC/ARM_CM4F/port.c`; stack frame; FPU lazy stacking (`vstmdbeq`, `EXC_RETURN`). Solution: two tasks; one uses `volatile float f = 1.0f;` in a loop so FPU context is live; print `fpu` `DONE`. GDB: break `PendSV_Handler`. `src` can be the same with TODOs in comments to fill EXC_RETURN bits in README answers (no compile TODOs required — `src` runs the float task too, README questions are the work). Still ship `solution/NOTES.md` answering EXC_RETURN / lazy stacking.

- [ ] **Step 5: Lab 28 idle-task**

`configUSE_IDLE_HOOK 1` in overlay. `vApplicationIdleHook` in `app.c` sets a flag; a task sees flag and prints `idle` `DONE`. README: tickless not implemented. `src`: hook empty.

- [ ] **Step 6: `./scripts/lab.sh solution 24` … `28` each shows `DONE`**

- [ ] **Step 7: Commit**

```bash
git add CMakeLists.txt labs/24-freertos-config labs/25-tcb-ready-lists labs/26-systick-tick labs/27-context-switch labs/28-idle-task
git commit -m "Add Phase 3 internals labs 24-28."
```

---

### Task 14: Root README lab index and Phase 1 regression

**Files:**
- Modify: `README.md`

- [ ] **Step 1: Expand README** with table of labs 01–28 (id, slug, phase), setup, `lab.sh` commands, BSP API table (`uart_*`, `bsp_irq_*`, `BSP_IRQHandler`), explicit “no Cube HAL”.

- [ ] **Step 2: Run all Phase 1 checks**

```bash
for n in 01 02 03 04 05 06 07 08 09 10 11 12; do
  ./scripts/lab.sh check "$n"
done
```

Expected: 12 successful diffs.

- [ ] **Step 3: Smoke Phase 2–3 solutions**

```bash
for n in 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28; do
  timeout 8 ./scripts/lab.sh solution "$n" | tee /tmp/lab$n.log
  grep -q '^DONE$' /tmp/lab$n.log
done
```

Lab 16 **solution** must be `DONE`; do not use src. If a solution hangs, fix that lab before claiming v1 done.

- [ ] **Step 4: Commit**

```bash
git add README.md
git commit -m "Document all labs and verify Phase 1 UART checks."
```

---

## Self-review (spec coverage)

| Spec item | Task |
|-----------|------|
| install-env.sh + verify tools | 1 |
| CMake Ninja toolchain | 2, 5 |
| QEMU netduinoplus2 UART | 2, 5 |
| BSP UART + IRQ API, no Cube HAL | 2, 3 |
| CMSIS-Core + device header | 2, 3 |
| FreeRTOS V11.1.0 CM4F heap_4 | 4 |
| hooks strings / configASSERT | 4, 10 |
| lab.sh qemu/solution/check/gdb | 5 |
| Lab folder contract + 28 labs | 6–13 |
| Phase 1 UART check | 6–9, 14 |
| Phase 2 symptoms + GDB notes | 10–12 |
| Phase 3 kernel file walk + FPU + idle | 13 |
| Success: check 01–12 on solutions | 14 |

No Docker, no MPU, no +TCP, no host Unity tests — omitted on purpose.
