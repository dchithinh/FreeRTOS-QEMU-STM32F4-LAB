add_library(bsp STATIC
    startup.s
    system_stm32f4xx.c
    uart.c
)

target_include_directories(bsp PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(bsp PRIVATE -Wall -Wextra)