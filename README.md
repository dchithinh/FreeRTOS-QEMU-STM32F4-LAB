# FreeRTOS exercises (QEMU STM32F4)

A single repo of FreeRTOS **kernel** exercises that run on **QEMU STM32F4**, sequenced so you can master APIs (Phase 1 TODO labs), failure modes (Phase 2 fix broken firmware), and internals (Phase 3 read the kernel and port).

Study material lives in each lab **README**. Answers live in **`solution/`** next to each lab. There is **no STM32Cube HAL** — labs use CMSIS device headers and the tiny **`bsp/`** API only (`HAL_UART_*`, `HAL_Init`, and Cube drivers are not used).

## Setup

On Ubuntu / WSL2:

```bash
git submodule update --init --recursive
./scripts/install-env.sh
```

That script installs and verifies `cmake` (3.20+), `ninja`, `arm-none-eabi-gcc`, `gdb-multiarch` (or `arm-none-eabi-gdb`), and `qemu-system-arm`.

Labs need **QEMU 5 or newer** (`-M netduinoplus2`). Ubuntu 20.04’s package is 4.2 and will fail with `unsupported machine type`. Then:

```bash
./scripts/install-qemu.sh
```

That script copies or points at a QEMU that supports the board (into gitignored `.tools/bin`). Override with `FREERTOS_QEMU=/path/to/qemu-system-arm`.

Configure and try lab 01:

```bash
./scripts/lab.sh qemu 01
```

CMake uses **Ninja** and an out-of-tree `build/` directory (gitignored). Toolchain file: `cmake/gcc-arm-none-eabi.cmake`.

## QEMU and UART

- Machine: **`netduinoplus2`** (STM32F405, Cortex-M4F). Requires QEMU 5+.
- QEMU serial: **`-display none -serial stdio`** (see `scripts/run-qemu.sh`).
- Firmware talks on **USART1**; QEMU wires serial port 0 to your terminal. The BSP sets this up in `bsp/uart.c` (register-level, not Cube HAL).

Every lab should eventually print a final line **`DONE`** on UART so checks and smoke tests can detect hangs.

## `lab.sh` commands

Numeric id is `01` … `28`. The script resolves `labs/NN-*`.

| Command | Meaning |
|---------|---------|
| `./scripts/lab.sh qemu NN` | Build learner `src/` firmware and run QEMU (UART on stdio) |
| `./scripts/lab.sh solution NN` | Build `solution/` firmware and run QEMU |
| `./scripts/lab.sh check NN` | **Phase 1 only:** UART output vs `expected.log` (solution build) |
| `./scripts/lab.sh gdb NN` | QEMU with `-s -S`, then attach GDB on learner `src/`; see lab README for breakpoints |
| `./scripts/lab.sh gdb-solution NN` | Same, but debug the `solution/` firmware |

Equivalent CMake: `cmake --build build --target qemu-01`, `qemu-01-solution`, `check-01`, etc.

## BSP API (no Cube HAL)

| API | Role |
|-----|------|
| `uart_init()` | Enable USART1 for QEMU stdio |
| `uart_puts(const char *s)` | Blocking UART string (critical section around TX); **adds a `[   ms] ` prefix at each line start** |
| `uart_printf(const char *fmt, …)` | Tiny formatted output via `uart_puts` |
| `uart_timestamp_enable(int on)` | `0` disables the timestamp prefix, `1` re-enables |
| `bsp_irq_init(uint32_t nvic_priority)` | Enable the dedicated lab IRQ at NVIC priority |
| `bsp_irq_pend(void)` | Pend that IRQ from a task (QEMU enters the handler) |
| `void BSP_IRQHandler(void)` | **Implement in `app.c`** for ISR labs (`FromISR`, `portYIELD_FROM_ISR`) |

You may also use CMSIS **`NVIC_*`** and **`IRQn_Type`** (e.g. lab 15). **Do not** call STM32Cube HAL or define `SysTick_Handler` (the FreeRTOS port owns SysTick).

Headers: `bsp/uart.h`, `bsp/irq.h`.

## Timestamped UART output

`uart_puts` prefixes **every line** with the kernel tick count converted to milliseconds:

```
[     0] low
[     2] high
[     3] resumed
[     3] DONE
```

- Format is `[` + ms right-aligned in 6 columns + `] `. It is emitted once per line, even when a lab prints the text and the `\r\n` in two separate `uart_puts` calls (lab 09 does that).
- The value comes from `xTaskGetTickCount()`, so it shows **scheduler time**, not host wall-clock — that is what makes race/starvation/timing labs readable.
- Tick rate is read from the live SysTick reload, so labs that change `configTICK_RATE_HZ` (lab 24 runs at 100 Hz) still print correct milliseconds.
- Turn it off for plain output:

```c
uart_timestamp_enable(0);
uart_puts("no prefix here\r\n");
```

`./scripts/lab.sh check NN` strips the prefix before diffing, so `expected.log` stays free of timestamps.

The overflow/malloc hooks bypass `uart_puts` (they write USART1 registers directly, because they run in scheduler context where critical sections are illegal), so hook lines such as `HOOK STACK_OVERFLOW` have **no** timestamp.

## Rules that break the kernel

- **A task function must never return.** FreeRTOS has no return address for a task; falling off the end runs `prvTaskExitError()` (interrupts off, infinite spin), so every task stops and UART goes silent. End with `for (;;)`, or `vTaskDelete(NULL)` as the last statement.
- **Pass handles, not addresses of handles:** `vTaskDelete(h)`, `xTaskNotifyGive(h)` — `&h` is a `TaskHandle_t *` and corrupts memory.
- **Never call a non-`FromISR` API from an ISR** (labs 14–15 show the failure).
- **Do not block in a timer callback** (lab 21): the timer daemon task serves every timer.
- **Do not define `SysTick_Handler`** — the FreeRTOS port owns SysTick.

## Debug mode (GDB)

Every target — learner `src/` and `solution/` — is built with **`-g3 -gdwarf-4`**, and CMake defaults to `CMAKE_BUILD_TYPE=Debug` (still `-O0`, so timing-sensitive labs behave the same). Source lines, locals, and kernel symbols are all available:

```bash
./scripts/lab.sh gdb 01            # breakpoints in labs/01-tasks/src/app.c
./scripts/lab.sh gdb-solution 01   # breakpoints in labs/01-tasks/solution/app.c
```

That starts QEMU with `-s -S` and attaches `gdb-multiarch`. Useful commands:

```gdb
break paused
continue
info locals
print xTickCount
```

Outside `lab.sh`, use the resolved QEMU (the PATH one on Ubuntu 20.04 is 4.2 and has no `netduinoplus2`):

```bash
.tools/bin/qemu-system-arm -M netduinoplus2 -display none -serial stdio -s -S \
  -kernel build/lab_01_solution
```

## Lab index (28 labs)

| ID | Slug | Phase |
|----|------|-------|
| 01 | `tasks` | 1 — APIs |
| 02 | `timing` | 1 |
| 03 | `queues` | 1 |
| 04 | `semaphores` | 1 |
| 05 | `mutex` | 1 |
| 06 | `notifications` | 1 |
| 07 | `event-groups` | 1 |
| 08 | `timers` | 1 |
| 09 | `stream-message-buffers` | 1 |
| 10 | `isr-apis` | 1 |
| 11 | `static-alloc` | 1 |
| 12 | `heap` | 1 |
| 13 | `configassert` | 2 — Issues |
| 14 | `isr-wrong-api` | 2 |
| 15 | `nvic-syscall-priority` | 2 |
| 16 | `stack-overflow` | 2 |
| 17 | `priority-inversion` | 2 |
| 18 | `deadlock` | 2 |
| 19 | `starvation` | 2 |
| 20 | `heap-leak` | 2 |
| 21 | `timer-callback-blocks` | 2 |
| 22 | `data-race` | 2 |
| 23 | `critical-vs-mutex` | 2 |
| 24 | `freertos-config` | 3 — Internals |
| 25 | `tcb-ready-lists` | 3 |
| 26 | `systick-tick` | 3 |
| 27 | `context-switch` | 3 |
| 28 | `idle-task` | 3 |

- **Phase 1:** fill in `src/app.c`; pass `./scripts/lab.sh check NN` against golden `expected.log`.
- **Phase 2:** broken `src/` — fix until README symptoms are gone; use GDB notes in the lab README.
- **Phase 3:** read kernel/port paths in the README; small exercises; GDB notes where listed.

Kernel: **FreeRTOS-Kernel V11.1.0** git submodule at `third_party/FreeRTOS-Kernel`, **heap_4**, CM4F port. Details: `docs/superpowers/specs/2026-11-17-freertos-exercises-design.md`.