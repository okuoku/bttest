cmake_minimum_required(VERSION 3.0)
project(bttest)

add_definitions(
    -ggdb
    )

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBUSB REQUIRED libusb-1.0)

include_directories(${LIBUSB_INCLUDE_DIRS})
link_directories(${LIBUSB_LIBRARY_DIRS})

include(./btstack_srcs.cmake)

set(btstack_srcs)
foreach(e ${btstack_core_srcs} ${btstack_libusb_srcs} ${btstack_posix_srcs})
    list(APPEND btstack_srcs
        btstack/${e})
endforeach()

include_directories(
    btstack/src
    btstack/3rd-party/rijndael
    btstack/3rd-party/micro-ecc
    btstack/3rd-party/tinydir # POSIX
    )
include_directories(cfg)

add_library(btstack STATIC ${btstack_srcs})


target_link_libraries(btstack
    ${LIBUSB_LIBRARIES}
    )

include_directories(
    btstack/platform/posix
    )
add_executable(btcheck btcheck.c)
target_link_libraries(btcheck btstack)

add_executable(streamer 
    nordic_spp_le_streamer.c
    examples_main.c
    )

target_link_libraries(streamer btstack)

add_executable(central
    central_nrf_spp.c
    examples_main.c
    )

target_link_libraries(central btstack)
target_compile_definitions(central
    PRIVATE
    -DTEST_CENTRAL)
