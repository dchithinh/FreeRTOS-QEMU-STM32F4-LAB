cmake_minimum_required(VERSION 3.20)
project(freertos_labs C ASM)

add_subdirectory(bsp)
include(${CMAKE_SOURCE_DIR}/cmake/FreeRTOS.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/AddLab.cmake)

add_freertos_lab(01 tasks)