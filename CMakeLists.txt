# Specify the minimum version for CMake

cmake_minimum_required(VERSION 3.11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-unused-parameter -g")
set(CMAKE_LINKER_FLAGS_DEBUG " -fno-omit-frame-pointer -fsanitize=address")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

project(kurve)

find_package(Freetype REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(OpenGL REQUIRED)

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

# Configure the json settings files
file(GLOB SETTINGS
    "${CMAKE_SOURCE_DIR}/settings/players.json"
)
file(COPY ${SETTINGS} DESTINATION "${CMAKE_BINARY_DIR}/settings")

set (PROJECT_SOURCES main.cpp)

add_executable(kurve ${all_SRCS})
link_libraries("-lglfw -ldl -lm")
target_link_libraries(
    kurve PRIVATE m glfw dl
    freetype 
    nlohmann_json::nlohmann_json
)

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
    add_compile_definitions(DEBUG)
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
install(FILES ${SETTINGS} DESTINATION "${files_dest}/settings")
install(FILES "kurve.png" DESTINATION "${files_dest}")

install(
    FILES kurve.desktop
    PERMISSIONS WORLD_EXECUTE WORLD_WRITE WORLD_READ
    DESTINATION ~/.local/share/applications
)