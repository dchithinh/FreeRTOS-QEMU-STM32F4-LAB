add_library(bsp STATIC
    startup.s
    system_stm32f4xx.c
    uart.c
    irq.c
    hooks.c
)

target_include_directories(bsp PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/third_party/cmsis/Include
)

target_include_directories(bsp PRIVATE
    ${CMAKE_SOURCE_DIR}/cmake/freertos_kernel_build
)

target_compile_options(bsp PRIVATE -Wall -Wextra -g3 -gdwarf-4)