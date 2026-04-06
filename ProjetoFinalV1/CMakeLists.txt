# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(ProjetoFinalV1 C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()


add_executable(ProjetoFinalV1 ProjetoFinalV1.c hardware_config.c ssd1306.c matriz_led.c)

pico_set_program_name(ProjetoFinalV1 "ProjetoFinalV1")
pico_set_program_version(ProjetoFinalV1 "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(ProjetoFinalV1 1)
pico_enable_stdio_usb(ProjetoFinalV1 1)

# Add the standard library to the build
target_link_libraries(ProjetoFinalV1 pico_stdlib)

# Add the standard include files to the build
target_include_directories(ProjetoFinalV1 PRIVATE ${CMAKE_CURRENT_LIST_DIR})

# Add any user requested libraries (ADC incluído para o Joystick)
target_link_libraries(ProjetoFinalV1 
        hardware_i2c
        hardware_dma
        hardware_watchdog
        hardware_adc 
        hardware_pwm
        hardware_pio   

    pico_cyw43_arch_lwip_threadsafe_background
    pico_lwip_mqtt
  
        pico_cyw43_arch_lwip_threadsafe_background
)


pico_generate_pio_header(ProjetoFinalV1 ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio)

pico_add_extra_outputs(ProjetoFinalV1)