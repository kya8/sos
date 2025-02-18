cmake_minimum_required(VERSION 3.20.0 FATAL_ERROR)

project(test
LANGUAGES C
)

option(ENABLE_TESTS "Build tests." ON)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH  ${CMAKE_CURRENT_BINARY_DIR}/lib)

if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -pedantic)
endif()

#file(GLOB SRC_FILES CONFIGURE_DEPENDS "*.h" "*.cpp" "*.hpp" "*.c")

set(GEN_HEADER_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
if(CMAKE_C_BYTE_ORDER STREQUAL BIG_ENDIAN)
set(SOS_CMAKE_ENDIAN_IS_BE 1)
else()
set(SOS_CMAKE_ENDIAN_IS_BE 0)
endif()
configure_file(sos_endian.h.in ${GEN_HEADER_DIR}/sos_endian.h @ONLY)

add_library(sos STATIC sos.h sos.c ${GEN_HEADER_DIR}/sos_endian.h)
target_include_directories(sos PUBLIC ${GEN_HEADER_DIR} .)

#-------- Tests

if(ENABLE_TESTS)
include(CTest)
add_subdirectory(test)
endif()
