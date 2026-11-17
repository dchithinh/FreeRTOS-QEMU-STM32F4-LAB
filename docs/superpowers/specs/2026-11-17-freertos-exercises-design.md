# FreeRTOS exercises (QEMU STM32F4) — design

Date: 2026-11-17  
Status: approved for spec review  
Scope: kernel track only (v1)

## Goal

A single repo of FreeRTOS **kernel** exercises that run on **QEMU STM32F4**, sequenced so the author can master:

1. **APIs** (recall by filling TODOs)
2. **Failure modes** (fix broken firmware)
3. **Internals** (read the kernel/port; small from-scratch or annotation work)

Study material lives in each lab **README** (why the lab exists, what to memorize). A **`solution/`** folder sits next to each exercise. No Anki decks, no global interview pack, no CMSIS-RTOS wrapper.

A later track (MPU, tickless idle, +TCP/+FAT, tracing) is explicitly **out of v1**.

## Approach

**Shared board support + numbered labs** (not one mega-firmware, not a full kernel copy per lab).

- One BSP, one vendored FreeRTOS-Kernel LTS tree, one **CMake** project (Ninja).
- Each lab may overlay `FreeRTOSConfig.h`.
- Work happens in `labs/NN-slug/src/`. Answers live in `labs/NN-slug/solution/`.

## Repository layout

```text
free-rtos/
  README.md
  CMakeLists.txt            # labs as targets; qemu / check / gdb custom targets
  cmake/
    gcc-arm-none-eabi.cmake # toolchain file
    FreeRTOS.cmake
  scripts/
    install-env.sh          # Ubuntu/WSL2: cmake, ninja, gcc-arm-none-eabi, qemu, gdb
    lab.sh                  # ./scripts/lab.sh qemu 01  (configure if needed, then build/run)
    run-qemu.sh
    check-uart.sh           # Phase 1: UART vs expected.log
  bsp/
    startup.s
    linker.ld
    uart.c / uart.h
    irq.c / irq.h           # dedicated IRQ labs may handle
    hooks.c                 # assert / malloc-failed / stack-overflow hooks
    FreeRTOSConfig.default.h
  third_party/
    FreeRTOS-Kernel/        # one LTS snapshot, all labs link this
    cmsis/                  # CMSIS-Core + STM32F405 device headers (NVIC, IRQn). Not STM32Cube HAL.
  labs/
    NN-slug/
      README.md
      FreeRTOSConfig.h
      src/app.c
      solution/app.c
      solution/NOTES.md
      expected.log          # Phase 1 only
  docs/superpowers/specs/   # this document
  build/                    # gitignored
```

Root `README.md` tells the reader: run `scripts/install-env.sh`, then `./scripts/lab.sh qemu 01`.

## Environment and target

### Host

WSL2 Linux (Ubuntu packages). `scripts/install-env.sh` installs and then **verifies**:

- `cmake` (3.20+)
- `ninja`
- `arm-none-eabi-gcc`
- `gdb-multiarch` (or `arm-none-eabi-gdb` if that is what the distro provides)
- `qemu-system-arm`

If a binary is missing after install, the script exits non-zero and names the package. **No Docker in v1.** GNU Make is not required.

### QEMU machine

- Machine: `netduinoplus2` (STM32F405, Cortex-M4F with FPU).
- Used: UART to stdio, SysTick, NVIC, one dedicated device IRQ for ISR labs.
- Not used: Ethernet, USB, sensors, **STM32Cube HAL** (`HAL_UART_*`, `HAL_GPIO_*`, `HAL_Init`).

Labs must not depend on hardware QEMU does not model.

### BSP API (what labs may call)

This is the “HAL” for exercises: a **tiny stable API in `bsp/`**, plus CMSIS `NVIC_*` / `IRQn` types. Labs never vendor Cube HAL.

| API | Use in labs |
|-----|-------------|
| `uart_puts` / `uart_printf` | All labs: UART output |
| `bsp_irq_init(priority)` | Enable the dedicated IRQ at an NVIC priority |
| `bsp_irq_pend(void)` | Pend that IRQ from a task (so QEMU actually enters the handler) |
| `void BSP_IRQHandler(void)` | **You implement this** in `app.c` (weak default in BSP does nothing). ISR labs put `xQueueSendFromISR` / `portYIELD_FROM_ISR` here |
| CMSIS `NVIC_SetPriority` / `NVIC_EnableIRQ` | Lab 15 (syscall priority) may use these directly to mis-configure then fix |

SysTick is owned by the FreeRTOS port. Labs must not define `SysTick_Handler`.

Startup provides a weak `BSP_IRQHandler` alias to a real unused STM32 vector that QEMU will take when pending (e.g. `EXTI0_IRQHandler`). Implementation picks one IRQn documented in `bsp/irq.h`.

### Image

- Linker script matches this machine’s flash/RAM map.
- Heap: `heap_4` in the BSP heap region.
- `configCPU_CLOCK_HZ` and `configTICK_RATE_HZ` (1000 Hz) are fixed in the default config unless a lab overlay changes them.

### Build and run

CMake is the build system of record. A toolchain file selects `arm-none-eabi-gcc`. Generator: **Ninja**. Out-of-tree build directory: `build/` (gitignored).

Each lab is two firmware targets (ELF + `.bin`):

- `lab_NN` — learner `src/`
- `lab_NN_solution` — `solution/`

Custom targets wrap QEMU/GDB. Day-to-day command is `scripts/lab.sh` so you do not type generator flags:

| Command | Meaning |
|--------|---------|
| `./scripts/lab.sh qemu 01` | Configure if needed, build `lab_01`, boot QEMU, UART on stdio |
| `./scripts/lab.sh solution 01` | Same using `lab_01_solution` |
| `./scripts/lab.sh check 01` | Phase 1 only: UART vs `expected.log` |
| `./scripts/lab.sh gdb 15` | QEMU `-s -S` plus GDB; lab README lists breakpoints |

`01` is the numeric prefix. The script resolves `labs/01-*`. Equivalent CMake: `cmake --build build --target qemu-01`.

No root `Makefile`. Lab READMEs document `./scripts/lab.sh`, not Make.

Every lab prints a final UART line `DONE` so `check-uart.sh` can time out if QEMU hangs.

## Lab folder contract

Required in every lab:

- `README.md`
- `FreeRTOSConfig.h` (overlay; may be a copy of the default)
- `src/app.c` (the learner edits this)
- `solution/app.c` (working tree, same config)
- `solution/NOTES.md` (why this solution, pitfalls — not a copy of the README)

Phase 1 also has `expected.log` (stable lines, no jittery timestamps).

### README headings (mandatory, in this order)

1. **Why this lab**
2. **What to memorize**
3. **Your job**
4. **How to run**
5. **Done when**
6. **GDB** — Phase 2 and 3 only (2–4 breakpoints and what to inspect)

### Pedagogy by phase

| Phase | `src/` | Check |
|-------|--------|--------|
| 1 APIs | Stubs / TODOs | `./scripts/lab.sh check NN` UART golden log |
| 2 Issues | Compiles and **misbehaves**; learner patches `src/` | Symptom in README gone; GDB notes; no golden log |
| 3 Internals | Read kernel/port; small from-scratch or annotation | README “done when”; GDB notes |

Labs may use the FreeRTOS API and the BSP API above. No STM32Cube HAL in `src/` or `solution/`.

## Firmware hooks and asserts

- `configASSERT` is **enabled** for every lab. Failed assert: infinite loop (GDB-catchable), no silent reset.
- `vApplicationMallocFailedHook` prints `HOOK MALLOC_FAILED` then hangs. `vApplicationStackOverflowHook` prints `HOOK STACK_OVERFLOW` then hangs.
- Phase 1 labs must not hit those hooks.
- Phase 2 stack-overflow and heap-exhaustion labs **must** hit the matching hook before the fix.
- Idle/timer task allocation failure at `vTaskStartScheduler` is documented in labs 01 and 12.

## Kernel version

Vendored **FreeRTOS-Kernel LTS** current at the time `third_party/` is added. All labs use that one tree. Labs change behavior via `FreeRTOSConfig.h` overlays, not by forking the kernel.

## Curriculum (v1, 28 labs)

Full kernel track: 12 API labs, 11 issue labs, 5 internals labs. The “27” figure from the discussion dropped idle as its own lab; **idle stays a lab**. Extra APIs (suspend/delete, stream-buffer writer rule, malloc hook, FPU) are folded into existing labs, not omitted.

### Phase 1 — APIs (fill-in, UART check)

| ID | Slug | Memorize / cover |
|----|------|------------------|
| 01 | `tasks` | `xTaskCreate`, stack in **words**, priority, states (Running/Ready/Blocked/Suspended/Deleted), `vTaskDelay`, `vTaskSuspend` / `vTaskResume` / `vTaskDelete`, scheduler start / idle+timer memory |
| 02 | `timing` | `vTaskDelay` vs `vTaskDelayUntil`, tick, `pdMS_TO_TICKS` |
| 03 | `queues` | create/send/recv, block time, copy semantics, length × item size |
| 04 | `semaphores` | binary vs counting; give/take; counting as resource pool |
| 05 | `mutex` | take/give, **not ISR-safe**, vs binary semaphore; short note on recursive mutex (not its own lab) |
| 06 | `notifications` | notify as binary / counting / mailbox; vs queue |
| 07 | `event-groups` | bits, wait-all vs wait-any, `xEventGroupSync` |
| 08 | `timers` | one-shot vs auto-reload, daemon task, do not block in callback |
| 09 | `stream-message-buffers` | stream vs message; trigger level; **single writer and single reader** |
| 10 | `isr-apis` | `FromISR`, `pxHigherPriorityTaskWoken`, `portYIELD_FROM_ISR` |
| 11 | `static-alloc` | `xTaskCreateStatic` and friends; NULL / omitted buffers |
| 12 | `heap` | `heap_4` on this BSP; `pvPortMalloc` failure; `vApplicationMallocFailedHook` |

### Phase 2 — Issues (broken firmware, GDB)

| ID | Slug | Bug class |
|----|------|-----------|
| 13 | `configassert` | API misuse caught by `configASSERT` |
| 14 | `isr-wrong-api` | Non-`FromISR` API in an ISR |
| 15 | `nvic-syscall-priority` | IRQ priority vs `configMAX_SYSCALL_INTERRUPT_PRIORITY` / `BASEPRI` |
| 16 | `stack-overflow` | Overflow + overflow hook / canary |
| 17 | `priority-inversion` | Inversion; mutex inheritance vs binary semaphore |
| 18 | `deadlock` | Two mutexes, opposite lock order |
| 19 | `starvation` | Equal-priority starve; `configUSE_TIME_SLICING` / preemption |
| 20 | `heap-leak` | Exhaustion; leak after `vTaskDelete` without freeing objects |
| 21 | `timer-callback-blocks` | Blocking in timer callback stalls the daemon |
| 22 | `data-race` | Shared data without mutex/critical section |
| 23 | `critical-vs-mutex` | `taskENTER_CRITICAL` vs `vTaskSuspendAll` vs mutex (ISR-safety, blocking) |

Lab 09: UART story uses **one** writer and **one** reader. The “two writers” failure is listed under **What to memorize** and `solution/NOTES.md` (not a separate lab).

### Phase 3 — Internals (read the port)

| ID | Slug | Open |
|----|------|------|
| 24 | `freertos-config` | Every knob this BSP uses; what a wrong value does |
| 25 | `tcb-ready-lists` | TCB; ready lists; priority as list index |
| 26 | `systick-tick` | SysTick on Cortex-M4; who increments `xTickCount` |
| 27 | `context-switch` | PendSV/SVC in the STM32F4 port; stack frame; **FPU lazy stacking** |
| 28 | `idle-task` | Idle task, `configUSE_IDLE_HOOK`; tickless idle is **not** implemented (concept only in README) |

**28 labs total:** 01–12 Phase 1, 13–23 Phase 2, 24–28 Phase 3.

## Testing

- **Automated in v1:** Phase 1 `./scripts/lab.sh check NN` only (UART `diff`).
- **Manual:** Phase 2–3 against README symptoms; GDB optional but documented.
- **CI (optional later):** Phase 1 checks only. Not required to ship v1.
- **Not in v1:** Unity/CMock, host FreeRTOS POSIX port tests, real Nucleo/STM32 hardware jobs.

`./scripts/lab.sh check NN` fails on log mismatch or if `DONE` never appears (timeout).

## Out of v1

- MPU, tickless idle, FreeRTOS+TCP, +FAT, CLI, Tracealyzer
- CMSIS-RTOS API
- Docker
- Queue sets, co-routines, TLS, task tags
- Recursive mutex as its own lab (note in lab 05 only)
- Second “production extras” track (hooks, more config pitfalls as a **series**) — may reuse this BSP later

## Success criteria

v1 is done when:

1. `scripts/install-env.sh` succeeds on Ubuntu/WSL2 and the four binaries exist.
2. `./scripts/lab.sh qemu 01` prints UART including `DONE`.
3. All 28 labs exist with the folder contract and solution trees.
4. All Phase 1 labs pass `./scripts/lab.sh check NN` using **solution** builds (proves goldens).
5. Each README has Why / Memorize / Job / Run / Done when (and GDB for Phase 2–3).

## Non-goals

- STM32Cube HAL / a general STM32 peripheral course (CMSIS device headers + `bsp/` only)
- Production product firmware
- Teaching QEMU internals beyond “run this machine and attach GDB”
