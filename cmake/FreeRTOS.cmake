set(FREERTOS_KERNEL_DIR ${CMAKE_SOURCE_DIR}/third_party/FreeRTOS-Kernel)
set(FREERTOS_KERNEL_TASKS_C ${FREERTOS_KERNEL_DIR}/tasks.c)
if(NOT EXISTS "${FREERTOS_KERNEL_TASKS_C}")
    message(FATAL_ERROR
        "FreeRTOS kernel not found: ${FREERTOS_KERNEL_TASKS_C}\n"
        "Run: git submodule update --init --recursive")
endif()

set(FREERTOS_KERNEL_SOURCES
    ${FREERTOS_KERNEL_DIR}/tasks.c
    ${FREERTOS_KERNEL_DIR}/queue.c
    ${FREERTOS_KERNEL_DIR}/list.c
    ${FREERTOS_KERNEL_DIR}/timers.c
    ${FREERTOS_KERNEL_DIR}/event_groups.c
    ${FREERTOS_KERNEL_DIR}/stream_buffer.c
    ${FREERTOS_KERNEL_DIR}/portable/GCC/ARM_CM4F/port.c
    ${FREERTOS_KERNEL_DIR}/portable/MemMang/heap_4.c
)

function(add_freertos_kernel_library LIB_NAME CONFIG_INCLUDE_DIR)
    add_library(${LIB_NAME} STATIC ${FREERTOS_KERNEL_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC
        ${FREERTOS_KERNEL_DIR}/include
        ${FREERTOS_KERNEL_DIR}/portable/GCC/ARM_CM4F
        ${CMAKE_SOURCE_DIR}/bsp/include
        ${CMAKE_SOURCE_DIR}/third_party/cmsis/Include
    )

    # FreeRTOSConfig.h for this lab/solution must be found before the default.
    target_include_directories(${LIB_NAME} PRIVATE
        ${CONFIG_INCLUDE_DIR}
    )

    target_compile_options(${LIB_NAME} PRIVATE -Wall -Wextra)
endfunction()

target_include_directories(bsp PRIVATE
    ${FREERTOS_KERNEL_DIR}/include
    ${FREERTOS_KERNEL_DIR}/portable/GCC/ARM_CM4F
)