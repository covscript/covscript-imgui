# covscript-imgui

Dear ImGui bindings for Covariant Script — create immediate-mode GUI applications.

## Implementations

### Cross-platform

- **GLFW + OpenGL 3.0** → `imgui.cse`
- **GLFW + OpenGL 2.0** → `imgui_legacy.cse`
- **SDL2 + SDL_Renderer** → `imgui_sdl.cse`

### Windows Native (Beta)

- **DirectX 11** → `imgui.cse`
- **DirectX 9** → `imgui_legacy.cse`

### Font

- **Font loading** → `imgui_font.cse`

## Dependencies

- [Covariant Script](https://github.com/covscript/covscript)
- GLFW3 and OpenGL (cross-platform backends)
- SDL2 (SDL backend, optional)

## Build

```bash
# Ubuntu
sudo apt install libglfw3-dev libsdl2-dev

# macOS
brew install glfw sdl2

# Build
mkdir build && cd build
cmake ..
cmake --build .
```

## Examples

See the [example games](https://github.com/covscript/covscript-imgui/tree/master/examples): breakout, flappy, hanoi, pong, sheep, snake, tetris, and a Vivaldi 2D game engine.

## License

Apache License 2.0 — see [LICENSE](./LICENSE)
