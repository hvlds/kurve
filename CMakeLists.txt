# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.11)
project(kurve)

# Set C++ standard and compilation flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-unused-parameter -g")
set(CMAKE_LINKER_FLAGS_DEBUG " -fno-omit-frame-pointer -fsanitize=address")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Project version
set(VERSION_MAJOR 0)
set(VERSION_MINOR 2)

# Dependencies
find_package(Freetype REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(OpenGL REQUIRED)

# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
)

# Source files
file(GLOB all_SRCS
    "${PROJECT_SOURCE_DIR}/include/*.h"
    "${PROJECT_SOURCE_DIR}/include/*.hpp"
    "${PROJECT_SOURCE_DIR}/src/*.c"
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
    ${FREETYPE_INCLUDE_DIRS}
)

# Shader files
file(GLOB SHADERS
    "${CMAKE_SOURCE_DIR}/static/shader/*.vs"
    "${CMAKE_SOURCE_DIR}/static/shader/*.fs"
)
file(COPY ${SHADERS} DESTINATION "${CMAKE_BINARY_DIR}/shader")

# Font files
file(GLOB FONTS
    "${CMAKE_SOURCE_DIR}/static/fonts/UbuntuMono-R.ttf"
    "${CMAKE_SOURCE_DIR}/static/fonts/UbuntuMono-B.ttf"
    "${CMAKE_SOURCE_DIR}/static/fonts/UbuntuMono-BI.ttf"
    "${CMAKE_SOURCE_DIR}/static/fonts/UbuntuMono-RI.ttf"
)
file(COPY ${FONTS} DESTINATION "${CMAKE_BINARY_DIR}/fonts")

# Settings files
file(GLOB SETTINGS
    "${CMAKE_SOURCE_DIR}/static/settings/players.json"
)
file(COPY ${SETTINGS} DESTINATION "${CMAKE_BINARY_DIR}/settings")

# Executable
add_executable(kurve ${all_SRCS})

# Link libraries
link_libraries("-lglfw -ldl -lm")
target_link_libraries(
    kurve PRIVATE 
    m 
    glfw 
    dl
    ${FREETYPE_LIBRARIES} 
    nlohmann_json::nlohmann_json
)
target_include_directories(kurve PRIVATE ${FREETYPE_INCLUDE_DIRS})

# Target properties
set_target_properties(kurve PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)

# Output locations
set(tool_dest "bin")
set(files_dest "share/kurve/static")

# Allow manual override of STATIC_FILES_PATH from command line
if (NOT DEFINED STATIC_FILES_PATH)
    if (CMAKE_BUILD_TYPE STREQUAL "Release" AND NOT CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        set(STATIC_FILES_PATH "${CMAKE_INSTALL_PREFIX}/${files_dest}")
    else()
        set(STATIC_FILES_PATH "${CMAKE_BINARY_DIR}")
    endif()
endif()

target_compile_definitions(kurve PRIVATE STATIC_FILES="${STATIC_FILES_PATH}")
message("STATIC_FILES = ${STATIC_FILES_PATH}")

# Debug mode settings
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("** Debug mode **")
    add_compile_definitions(DEBUG)
endif()

# Release mode settings
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    message("** Release mode **")

    # Generate .desktop file only if we are installing
    if (NOT CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        file(READ "template.desktop" filedata)
        file(WRITE  "kurve.desktop" "${filedata}Exec=${CMAKE_INSTALL_PREFIX}/${tool_dest}/kurve\n")
        file(READ "kurve.desktop" filedata)
        file(WRITE  "kurve.desktop" "${filedata}Icon=${CMAKE_INSTALL_PREFIX}/${files_dest}/kurve.png\n")
    endif()
endif()

# Snapcraft-specific path handling
if (CMAKE_BUILD_TYPE STREQUAL "SNAPCRAFT")
    message("** SNAPCRAFT mode **")
    set(STATIC_FILES_PATH "/snap/kurve/current/${files_dest}")
    target_compile_definitions(kurve PRIVATE STATIC_FILES="${STATIC_FILES_PATH}")
    message("STATIC_FILES = ${STATIC_FILES_PATH}")
endif()

# Install rules
install(TARGETS kurve DESTINATION "${tool_dest}")
install(FILES ${SHADERS} DESTINATION "${files_dest}/shader")
install(FILES ${FONTS} DESTINATION "${files_dest}/fonts")
install(FILES ${SETTINGS} DESTINATION "${files_dest}/settings")
install(FILES "kurve.png" DESTINATION "${files_dest}")

# Install .desktop file if not SNAPCRAFT
if (NOT CMAKE_BUILD_TYPE STREQUAL "SNAPCRAFT" AND NOT CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    install(
        FILES kurve.desktop
        PERMISSIONS WORLD_EXECUTE WORLD_WRITE WORLD_READ
        DESTINATION ~/.local/share/applications
    )
endif()

# DEB packaging
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set(CPACK_PACKAGE_DESCRIPTION "Remake of 'Achtung, die Kurve!'")
set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set(CPACK_PACKAGE_VENDOR "Hernán Valdés")
set(CPACK_PACKAGE_CONTACT "hfvaldesg@gmail.com")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libglfw3-dev,libfreetype-dev,nlohmann-json3-dev")
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${VERSION_MAJOR}.${VERSION_MINOR}")
include(CPack)
