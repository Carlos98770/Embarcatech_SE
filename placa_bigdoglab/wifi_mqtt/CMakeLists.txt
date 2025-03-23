# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")


# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(wifi_mqtt C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(wifi_mqtt src/wifi_mqtt.c src/mqtt.c)

pico_set_program_name(wifi_mqtt "wifi_mqtt")
pico_set_program_version(wifi_mqtt "0.1")

# Generate PIO header
pico_generate_pio_header(wifi_mqtt ${CMAKE_CURRENT_LIST_DIR}/blink.pio)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(wifi_mqtt 0)
pico_enable_stdio_usb(wifi_mqtt 1)

set(PICO_LWIP_ENABLE_MQTT 1)  # Habilita MQTT no LWIP


# Add the standard library to the build
target_link_libraries(wifi_mqtt
        pico_stdlib
        pico_lwip_mqtt
        )

# Add the standard include files to the build
target_include_directories(wifi_mqtt PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(wifi_mqtt 
        hardware_i2c
        hardware_pio
        pico_cyw43_arch_lwip_threadsafe_background
        )

pico_add_extra_outputs(wifi_mqtt)

