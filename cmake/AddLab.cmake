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