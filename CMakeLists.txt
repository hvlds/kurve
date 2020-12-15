# Specify the minimum version for CMake

cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-unused-parameter -g")
set(CMAKE_LINKER_FLAGS_DEBUG " -fno-omit-frame-pointer -fsanitize=address")

project(kurve)

find_package(Freetype REQUIRED)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
)
file(GLOB all_SRCS
    "${PROJECT_SOURCE_DIR}/include/*.h"
    "${PROJECT_SOURCE_DIR}/include/*.hpp"
    "${PROJECT_SOURCE_DIR}/src/*.c"
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
    ${FREETYPE_INCLUDE_DIRS}
)

# Configure the shader files
file(GLOB SHADERS
    "${CMAKE_SOURCE_DIR}/shader/*.vs"
    "${CMAKE_SOURCE_DIR}/shader/*.fs"
)
file(COPY ${SHADERS} DESTINATION "${CMAKE_BINARY_DIR}/shader")

# Configure the font files
file(GLOB FONTS
    "${CMAKE_SOURCE_DIR}/fonts/UbuntuMono-R.ttf"
)
file(COPY ${FONTS} DESTINATION "${CMAKE_BINARY_DIR}/fonts")

set (PROJECT_SOURCES main.cpp)

add_executable(kurve ${all_SRCS})
link_libraries("-lglfw -ldl -lm")
target_link_libraries(kurve m GL GLU glfw3 X11 Xxf86vm Xrandr pthread Xi dl Xinerama Xcursor freetype)

set_target_properties(kurve PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)

set(tool_dest "bin")
set(files_dest "share/kurve")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("** Debug mode **")
    target_compile_definitions(kurve PRIVATE STATIC_FILES="${CMAKE_BINARY_DIR}")
    message("STATIC_FILES = ${CMAKE_BINARY_DIR}")
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    message("** Release mode **")
    target_compile_definitions(kurve PRIVATE STATIC_FILES="${CMAKE_INSTALL_PREFIX}/${files_dest}")
    message("STATIC_FILES = ${files_dest}")
    file(READ "template.desktop" filedata)
    file(WRITE  "kurve.desktop" "${filedata}Exec=${CMAKE_INSTALL_PREFIX}/${tool_dest}/kurve\n")
    file(READ "kurve.desktop" filedata)
    file(WRITE  "kurve.desktop" "${filedata}Icon=${CMAKE_INSTALL_PREFIX}/${files_dest}/kurve.png\n")
endif (CMAKE_BUILD_TYPE STREQUAL "Release")

install(TARGETS kurve DESTINATION "${tool_dest}")

install(FILES ${SHADERS} DESTINATION "${files_dest}/shader")
install(FILES ${FONTS} DESTINATION "${files_dest}/fonts")
install(FILES "kurve.png" DESTINATION "${files_dest}")

install(
    FILES kurve.desktop
    PERMISSIONS WORLD_EXECUTE WORLD_WRITE WORLD_READ
    DESTINATION ~/.local/share/applications
)