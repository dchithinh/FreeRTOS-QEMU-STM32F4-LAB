function(add_freertos_lab ID SLUG)
    set(LAB_DIR ${CMAKE_SOURCE_DIR}/labs/${ID}-${SLUG})

    set(SOLUTION_CONFIG_DIR ${LAB_DIR})
    if(EXISTS ${LAB_DIR}/solution/FreeRTOSConfig.h)
        set(SOLUTION_CONFIG_DIR ${LAB_DIR}/solution)
    endif()

    add_freertos_kernel_library(freertos_kernel_lab_${ID} ${LAB_DIR})
    add_freertos_kernel_library(freertos_kernel_lab_${ID}_solution ${SOLUTION_CONFIG_DIR})

    add_executable(lab_${ID} ${LAB_DIR}/src/app.c)
    add_executable(lab_${ID}_solution ${LAB_DIR}/solution/app.c)

    target_include_directories(lab_${ID} PRIVATE ${LAB_DIR} ${CMAKE_SOURCE_DIR}/bsp/include)
    target_link_libraries(lab_${ID} PRIVATE freertos_kernel_lab_${ID} bsp)
    target_compile_options(lab_${ID} PRIVATE -g3 -gdwarf-4)
    target_link_options(lab_${ID} PRIVATE -T${CMAKE_SOURCE_DIR}/bsp/linker.ld
            "-Wl,-Map=$<TARGET_FILE_DIR:lab_${ID}>/lab_${ID}.map")

    set(SOLUTION_APP_INCLUDES ${LAB_DIR})
    if(EXISTS ${LAB_DIR}/solution/FreeRTOSConfig.h)
        set(SOLUTION_APP_INCLUDES ${LAB_DIR}/solution ${LAB_DIR})
    endif()
    target_include_directories(lab_${ID}_solution PRIVATE
            ${SOLUTION_APP_INCLUDES} ${CMAKE_SOURCE_DIR}/bsp/include)
    target_link_libraries(lab_${ID}_solution PRIVATE freertos_kernel_lab_${ID}_solution bsp)
    target_compile_options(lab_${ID}_solution PRIVATE -g3 -gdwarf-4)
    target_link_options(lab_${ID}_solution PRIVATE -T${CMAKE_SOURCE_DIR}/bsp/linker.ld
            "-Wl,-Map=$<TARGET_FILE_DIR:lab_${ID}_solution>/lab_${ID}_solution.map")

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