cmake_minimum_required(VERSION 3.20)
project(freertos_labs C ASM)

add_subdirectory(bsp)

add_executable(uart_smoke labs/_uart_smoke/main.c)
target_link_libraries(uart_smoke PRIVATE bsp)
target_link_options(uart_smoke PRIVATE
    -T${CMAKE_SOURCE_DIR}/bsp/linker.ld
    -Wl,--print-memory-usage
)