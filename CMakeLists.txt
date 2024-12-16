cmake_minimum_required(VERSION 3.28)

set(PROGRAM_NAME smart_smoke_detector)
set(PICO_BOARD pico_w)

SET(FREERTOS_KERNEL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib/FreeRTOS-Kernel)

include(pico_sdk_import.cmake)
include(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

project(smart_smoke_detector)

pico_sdk_init()

message("Running makefsdata python script")
execute_process(COMMAND
    py makefsdata.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
)

add_executable(${PROGRAM_NAME}
    main.c
)

target_include_directories(${PROGRAM_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    #${PICO_LWIP_PATH}/src/apps/altcp_tls/
    #${PICO_SDK_PATH}/lib/lwip/src/include
    #${PICO_SDK_PATH}/lib/lwip/src/include/arch
) #remove last 2

target_link_libraries(${PROGRAM_NAME}
    pico_cyw43_arch_lwip_threadsafe_background
    pico_lwip_http
    pico_stdlib
    hardware_adc
    FreeRTOS-Kernel-Heap4 
    #pico_lwip_mbedtls
    #pico_mbedtls
)#picohttps

add_definitions(-DPICO_CYW43_ARCH_THREADSAFE_BACKGROUND=1) # del that

pico_enable_stdio_usb(${PROGRAM_NAME} TRUE)
pico_enable_stdio_uart(${PROGRAM_NAME} FALSE)

pico_add_extra_outputs(${PROGRAM_NAME})