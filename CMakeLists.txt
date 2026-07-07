cmake_minimum_required(VERSION 3.16)

project(covscript-imgui)
include_directories(backends)
include_directories(include)
include_directories(include/imgui)

if (DEFINED ENV{CS_DEV_PATH})
    message("-- CovScript SDK detected at $ENV{CS_DEV_PATH}")
    include("$ENV{CS_DEV_PATH}/csbuild.cmake")
    include_directories($ENV{CS_DEV_PATH}/include)
    link_directories($ENV{CS_DEV_PATH}/lib)
else ()
    message(FATAL_ERROR "-- CovScript SDK not detected. Please set environment variable CS_DEV_PATH")
endif ()

set(IMGUI_SOURCE_CODE
        backends/imgui_impl_glfw.cpp
        res/default_font.cpp
        src/gl3w.c
        src/imgui.cpp
        src/imgui_demo.cpp
        src/imgui_draw.cpp
        src/imgui_widgets.cpp
        src/imgui_tables.cpp
        src/imgui_stdlib.cpp)

add_library(imgui STATIC ${IMGUI_SOURCE_CODE})
add_library(imgui_font_ext SHARED imgui_font.cpp)

add_library(imgui_ext SHARED backends/imgui_impl_opengl3.cpp imgui.cpp)
add_library(imgui_legacy_ext SHARED backends/imgui_impl_opengl2.cpp imgui.cpp)
target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GL3W)
target_compile_definitions(imgui_legacy_ext PRIVATE IMGUI_IMPL_GL2)

if (WIN32)
    target_compile_definitions(imgui PUBLIC IMGUI_ENABLE_WIN32_DEFAULT_IME_FUNCTIONS)
    target_link_libraries(imgui glfw3 gdi32 opengl32 imm32)
endif ()

if (UNIX)
    find_package(glfw3 REQUIRED)
    if (APPLE)
        link_directories("/usr/local/lib")
        link_directories("/opt/local/lib")
        find_library(OPENGL_LIBS OpenGL)
        find_library(COCOA_LIBS Cocoa)
        find_library(IOKIT_LIBS IOKit)
        find_library(COREVIDEO_LIBS CoreVideo)
        target_link_libraries(imgui glfw ${OPENGL_LIBS} ${COCOA_LIBS} ${IOKIT_LIBS} ${COREVIDEO_LIBS})
    else ()
        target_link_libraries(imgui glfw GL)
    endif ()
endif ()

target_link_libraries(imgui_font_ext covscript imgui)
target_link_libraries(imgui_ext covscript imgui)
target_link_libraries(imgui_legacy_ext covscript imgui)

add_library(imgui_sdl_ext SHARED
    backends/imgui_impl_sdl2.cpp
    backends/imgui_impl_sdlrenderer2.cpp
    imgui.cpp)
target_compile_definitions(imgui_sdl_ext PRIVATE IMGUI_IMPL_SDL2)

if (WIN32)
    find_package(SDL2 REQUIRED)
    target_link_libraries(imgui_sdl_ext SDL2::SDL2-static)
elseif (APPLE)
    find_package(SDL2 REQUIRED)
    link_directories("/usr/local/lib")
    link_directories("/opt/local/lib")
    find_library(COCOA_LIBS Cocoa)
    find_library(IOKIT_LIBS IOKit)
    find_library(COREVIDEO_LIBS CoreVideo)
    target_link_libraries(imgui_sdl_ext SDL2::SDL2-static ${COCOA_LIBS} ${IOKIT_LIBS} ${COREVIDEO_LIBS})
else()
    find_package(SDL2 REQUIRED)
    target_link_libraries(imgui_sdl_ext SDL2::SDL2)
endif()

target_link_libraries(imgui_sdl_ext covscript imgui)

set_target_properties(imgui_sdl_ext PROPERTIES OUTPUT_NAME imgui_sdl)
set_target_properties(imgui_sdl_ext PROPERTIES PREFIX "")
set_target_properties(imgui_sdl_ext PROPERTIES SUFFIX ".cse")

set_target_properties(imgui_font_ext PROPERTIES OUTPUT_NAME imgui_font)
set_target_properties(imgui_font_ext PROPERTIES PREFIX "")
set_target_properties(imgui_font_ext PROPERTIES SUFFIX ".cse")

set_target_properties(imgui_ext PROPERTIES OUTPUT_NAME imgui)
set_target_properties(imgui_ext PROPERTIES PREFIX "")
set_target_properties(imgui_ext PROPERTIES SUFFIX ".cse")

set_target_properties(imgui_legacy_ext PROPERTIES OUTPUT_NAME imgui_legacy)
set_target_properties(imgui_legacy_ext PROPERTIES PREFIX "")
set_target_properties(imgui_legacy_ext PROPERTIES SUFFIX ".cse")