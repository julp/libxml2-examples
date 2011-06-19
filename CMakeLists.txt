cmake_minimum_required(VERSION 2.6)

find_package(LibXml2 REQUIRED)

message(STATUS LIBXML2_FOUND = ${LIBXML2_FOUND})

file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/*.c)

# configure_file(
#     "config.h.in"
#     "config.h"
# )

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${LIBXML2_INCLUDE_DIR})

foreach (SOURCE ${SOURCES})
    get_filename_component(BINARY ${SOURCE} NAME_WE)
    #message("${SOURCE} => ${BINARY}")
    add_executable(${BINARY} ${SOURCE})
    target_link_libraries(${BINARY} ${LIBXML2_LIBRARIES})
endforeach(SOURCE)
