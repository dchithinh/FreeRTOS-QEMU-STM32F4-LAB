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