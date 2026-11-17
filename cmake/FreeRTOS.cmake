set(FREERTOS_KERNEL_DIR ${CMAKE_SOURCE_DIR}/third_party/FreeRTOS-Kernel)

add_library(freertos_kernel STATIC
    ${FREERTOS_KERNEL_DIR}/tasks.c
    ${FREERTOS_KERNEL_DIR}/queue.c
    ${FREERTOS_KERNEL_DIR}/list.c
    ${FREERTOS_KERNEL_DIR}/timers.c
    ${FREERTOS_KERNEL_DIR}/event_groups.c
    ${FREERTOS_KERNEL_DIR}/stream_buffer.c
    ${FREERTOS_KERNEL_DIR}/portable/GCC/ARM_CM4F/port.c
    ${FREERTOS_KERNEL_DIR}/portable/MemMang/heap_4.c
)

target_include_directories(freertos_kernel PUBLIC
    ${FREERTOS_KERNEL_DIR}/include
    ${FREERTOS_KERNEL_DIR}/portable/GCC/ARM_CM4F
    ${CMAKE_SOURCE_DIR}/bsp/include
    ${CMAKE_SOURCE_DIR}/third_party/cmsis/Include
)

# Labs supply FreeRTOSConfig.h on their targets (PUBLIC not used). PRIVATE config
# is only for compiling this static library and bsp hook objects.
target_include_directories(freertos_kernel PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/freertos_kernel_build
)

target_compile_options(freertos_kernel PRIVATE -Wall -Wextra)

# bsp/hooks.c needs FreeRTOS types; labs link freertos_kernel explicitly.
target_include_directories(bsp PRIVATE
    ${FREERTOS_KERNEL_DIR}/include
    ${FREERTOS_KERNEL_DIR}/portable/GCC/ARM_CM4F
)