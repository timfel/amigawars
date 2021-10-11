if(DEFINED AMIGA_GCC_PREFIX)
       include(bebbo-toolchain.cmake)
else()
       if(NOT WIN32)
              message(FATAL_ERROR "Bartman's toolchain works only on Windows. Define AMIGA_GCC_PREFIX to point to Bebbo's toolchain")
       endif()
       # the toolchain usage redefines WIN32, so we provide another variable
       set(IS_WIN32 1)
       include(bartman-toolchain.cmake)
       # this is a workaround for a cmake bug when using nmake and gnumake on windows
       # see: https://gitlab.kitware.com/cmake/cmake/-/issues/21997
       set(CMAKE_DEPENDS_USE_COMPILER OFF)
endif()

cmake_minimum_required(VERSION 3.16)
project(amigawars)

add_subdirectory(deps/ACE ace)
include_directories(deps/ACE/include)

include_directories(
        ${PROJECT_SOURCE_DIR}/src
)

file(GLOB_RECURSE C_SOURCES src/*.c)
file(GLOB_RECURSE HEADERS src/*.h)

set(SOURCES ${C_SOURCES} ${HEADERS})

include(target-definition.cmake)

# resources
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/resources/imgs)
convertTileset(
       INTERLEAVED
       SOURCE ${PROJECT_SOURCE_DIR}/resources/for.png
       DESTINATION ${PROJECT_BINARY_DIR}/resources/imgs/for.bm
       SIZE 16
       TARGET ${PROJECT_EXECUTABLE}
       PALETTE ${PROJECT_BINARY_DIR}/resources/imgs/for.plt)
convertPalette(${PROJECT_EXECUTABLE} ${PROJECT_SOURCE_DIR}/resources/for.gpl ${PROJECT_BINARY_DIR}/resources/imgs/for.plt)

execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/resources/maps)
add_custom_command(
       TARGET ${PROJECT_EXECUTABLE} POST_BUILD
	COMMAND ${CMAKE_COMMAND} ARGS -E copy ${PROJECT_SOURCE_DIR}/resources/maps/game.map ${PROJECT_BINARY_DIR}/resources/maps/game.map
	COMMAND ${CMAKE_COMMAND} ARGS -E copy ${PROJECT_SOURCE_DIR}/resources/maps/game2.map ${PROJECT_BINARY_DIR}/resources/maps/game2.map
)

execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/resources/units)
convertBitmaps(
       INTERLEAVED
       TARGET ${PROJECT_EXECUTABLE}
       PALETTE ${PROJECT_BINARY_DIR}/resources/imgs/for.plt
       MASK_COLOR "#ff55ff"
       SOURCES ${PROJECT_SOURCE_DIR}/resources/spearthrower.png
       DESTINATIONS ${PROJECT_BINARY_DIR}/resources/units/spearthrower.bm
       MASKS ${PROJECT_BINARY_DIR}/resources/units/spearthrower.msk
)
