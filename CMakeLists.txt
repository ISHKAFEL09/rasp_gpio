cmake_minimum_required(VERSION 3.14)
project(rasp_gpio C)

set(CMAKE_C_STANDARD 11)
set(RASP_LINUX "/home/wang/Code/ldd/linux")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -include ${RASP_LINUX}/include/linux/kconfig.h")

add_definitions(-DKBUILD_MODNAME=\"rasp_gpio\")
add_definitions(-D__KERNEL__=1)

include_directories(
        "${RASP_LINUX}/include",
        "${RASP_LINUX}/arch/arm/include",
        "${RASP_LINUX}/arch/arm64/include",
)

add_executable(rasp_gpio
            rasp_gpio.c
            rasp_gpio.h
        )
