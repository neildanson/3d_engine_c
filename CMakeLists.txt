cmake_minimum_required(VERSION 3.10)
project(3d_game VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)

# Use glad from system or fetch it
include(FetchContent)

# Fetch GLAD
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG v0.1.36
)
FetchContent_MakeAvailable(glad)

# Engine library
set(ENGINE_SOURCES
    engine/core/engine.c
    engine/input/input.c
    engine/renderer/mesh.c
    engine/renderer/shader.c
    engine/renderer/camera.c
    engine/resource/obj_loader.c
    engine/resource/terrain.c
)

set(ENGINE_HEADERS
    engine/core/types.h
    engine/core/engine.h
    engine/math/vec2.h
    engine/math/vec3.h
    engine/math/mat4.h
    engine/input/input.h
    engine/renderer/mesh.h
    engine/renderer/shader.h
    engine/renderer/camera.h
    engine/resource/obj_loader.h
    engine/resource/terrain.h
)

add_library(engine STATIC ${ENGINE_SOURCES} ${ENGINE_HEADERS})
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(engine PUBLIC OpenGL::GL glfw glad)

# Game library
set(GAME_SOURCES
    game/player.c
    game/enemy.c
    game/game.c
)

set(GAME_HEADERS
    game/player.h
    game/enemy.h
    game/game.h
)

add_library(game STATIC ${GAME_SOURCES} ${GAME_HEADERS})
target_include_directories(game PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(game PUBLIC engine)

# Main executable
add_executable(3d_game main.c)
target_link_libraries(3d_game PRIVATE game engine)

# Copy assets to build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Platform-specific settings
if(WIN32)
    target_compile_definitions(engine PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_definitions(game PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

# Math library on Unix
if(UNIX AND NOT APPLE)
    target_link_libraries(engine PUBLIC m)
endif()

# Install rules
install(TARGETS 3d_game DESTINATION bin)
install(DIRECTORY assets DESTINATION share/3d_game)
