include(common-toolchain.cmake)

set(TOOLCHAIN_PATH "${AMIGA_GCC_PREFIX}" CACHE STRING "")
set(TOOLCHAIN_PREFIX "m68k-amigaos" CACHE STRING "")
set(M68K_CPU "68000" CACHE STRING "")
set(M68K_FPU "soft" CACHE STRING "")

message(STATUS "TOOLCHAIN_PATH: ${TOOLCHAIN_PATH}")
message(STATUS "TOOLCHAIN_PREFIX: ${TOOLCHAIN_PREFIX}")
message(STATUS "M68K_CPU: ${M68K_CPU}")
message(STATUS "M68K_FPU: ${M68K_FPU}")

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/deps/AmigaCMakeCrossToolchains/m68k-amigaos.cmake" CACHE STRING "")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DAMIGA -Wall -Wextra -fomit-frame-pointer")
