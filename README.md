# 3D Game Engine in C

A modular 3D game engine built with OpenGL and GLFW, featuring terrain generation, character control, and enemy AI.

## Features

### Engine (Modular, Reusable)
- **Math Library**: Vec2, Vec3, Mat4 with common operations
- **Rendering**: OpenGL 3.3+ Core Profile renderer with shader support
- **Input**: Keyboard and mouse input handling
- **Camera**: First-person camera with mouse look
- **Resource Loading**: OBJ file loader, terrain generation

### Game
- **3D Terrain**: Procedurally generated from heightmap using Perlin noise
- **Player**: WASD movement, mouse camera control, jumping (Space), health system
- **Enemies**: AI-controlled enemies that chase and attack the player when close
- **Collision**: Terrain collision for player and enemies

## Controls

- **W/A/S/D** - Move forward/left/backward/right
- **Mouse** - Look around
- **Space** - Jump
- **ESC** - Toggle mouse capture

## Building

### Prerequisites
- CMake 3.10+
- OpenGL 3.3+ compatible graphics driver
- GLFW3 development libraries

### Linux (Ubuntu/Debian)
```bash
# Install dependencies
sudo apt-get install cmake libglfw3-dev libgl1-mesa-dev

# Build
mkdir build && cd build
cmake ..
make

# Run
./3d_game
```

### Windows (Visual Studio)
```bash
# Using vcpkg for dependencies
vcpkg install glfw3

# Build
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg-root]/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release

# Run
Release\3d_game.exe
```

### macOS
```bash
# Install dependencies
brew install cmake glfw

# Build
mkdir build && cd build
cmake ..
make

# Run
./3d_game
```

## Project Structure

```
├── engine/                 # Core engine (modular, game-independent)
│   ├── core/              # Engine core (types, main loop)
│   │   ├── types.h        # Common type definitions
│   │   ├── engine.h       # Engine interface
│   │   └── engine.c       # Engine implementation
│   ├── math/              # Math library
│   │   ├── vec2.h         # 2D vector
│   │   ├── vec3.h         # 3D vector
│   │   └── mat4.h         # 4x4 matrix
│   ├── input/             # Input handling
│   │   ├── input.h
│   │   └── input.c
│   ├── renderer/          # Rendering system
│   │   ├── mesh.h/.c      # Mesh handling
│   │   ├── shader.h/.c    # Shader handling
│   │   └── camera.h/.c    # Camera system
│   └── resource/          # Resource loading
│       ├── obj_loader.h/.c # OBJ file parser
│       └── terrain.h/.c    # Terrain generation
├── game/                   # Game-specific code
│   ├── player.h/.c        # Player controller
│   ├── enemy.h/.c         # Enemy AI
│   └── game.h/.c          # Game logic
├── assets/                 # Game assets
│   └── models/            # 3D models (OBJ format)
│       ├── player.obj     # Player character model
│       └── enemy.obj      # Enemy model
├── main.c                 # Entry point
├── CMakeLists.txt         # CMake build configuration
└── README.md              # This file
```

## Engine Architecture

The engine is designed to be modular and independent of the game logic:

1. **Core Layer**: Window management, main loop, timing
2. **Math Layer**: Vector/matrix operations for 3D graphics
3. **Input Layer**: Keyboard and mouse handling
4. **Renderer Layer**: OpenGL abstraction (meshes, shaders, camera)
5. **Resource Layer**: Asset loading (OBJ, terrain)

The game layer builds on top of the engine, implementing:
- Player character with physics
- Enemy AI with state machine
- Game state management

## Creating Your Own Game

The engine can be reused for other games by:

1. Include the engine headers
2. Create engine instance with configuration
3. Implement your game logic using engine systems
4. Call engine update/render in your game loop

Example:
```c
#include "engine/core/engine.h"
#include "engine/renderer/mesh.h"

int main() {
    EngineConfig config = engine_default_config();
    config.window_title = "My Game";
    
    Engine* engine = engine_create(&config);
    
    while (!engine_should_close(engine)) {
        engine_poll_events(engine);
        engine_begin_frame(engine);
        
        // Your game logic here
        
        engine_end_frame(engine);
    }
    
    engine_destroy(engine);
    return 0;
}
```

## License

MIT License