cmake_minimum_required(VERSION 3.10)

project(covscript-imgui)
include_directories(../backends)
include_directories(../include)
include_directories(../include/imgui)

if (DEFINED ENV{CS_DEV_PATH})
    message("-- CovScript SDK detected at $ENV{CS_DEV_PATH}")
    include("$ENV{CS_DEV_PATH}/csbuild.cmake")
    include_directories($ENV{CS_DEV_PATH}/include)
    link_directories($ENV{CS_DEV_PATH}/lib)
else ()
    message(FATAL_ERROR "-- CovScript SDK not detected. Please set environment variable CS_DEV_PATH")
endif ()

set(IMGUI_SOURCE_CODE
        ../backends/imgui_impl_win32.cpp
        ../res/default_font.cpp
        ../src/gl3w.c
        ../src/imgui.cpp
        ../src/imgui_demo.cpp
        ../src/imgui_draw.cpp
        ../src/imgui_widgets.cpp
        ../src/imgui_tables.cpp
        ../src/imgui_stdlib.cpp)

add_library(imgui STATIC ${IMGUI_SOURCE_CODE})
add_library(imgui_font_ext SHARED ../imgui_font.cpp)
add_library(imgui_ext SHARED ../backends/imgui_impl_dx11.cpp ../src/WICTextureLoader.cpp ../imgui.cpp)
add_library(imgui_legacy_ext SHARED ../backends/imgui_impl_dx9.cpp ../imgui.cpp)

target_compile_definitions(imgui PUBLIC IMGUI_IMPL_WIN32)
target_compile_definitions(imgui PUBLIC IMGUI_ENABLE_WIN32_DEFAULT_IME_FUNCTIONS)
target_compile_definitions(imgui_legacy_ext PRIVATE IMGUI_IMPL_DX9)

target_link_libraries(imgui gdi32 dwmapi imm32)
target_link_libraries(imgui_ext d3d11 d3dcompiler gdi32 dwmapi)
target_link_libraries(imgui_legacy_ext d3d9 d3dx9 gdi32 dwmapi)

target_link_libraries(imgui_font_ext covscript imgui)
target_link_libraries(imgui_ext covscript imgui)
target_link_libraries(imgui_legacy_ext covscript imgui)

set_target_properties(imgui_font_ext PROPERTIES OUTPUT_NAME imgui_font)
set_target_properties(imgui_font_ext PROPERTIES PREFIX "")
set_target_properties(imgui_font_ext PROPERTIES SUFFIX ".cse")

set_target_properties(imgui_ext PROPERTIES OUTPUT_NAME imgui)
set_target_properties(imgui_ext PROPERTIES PREFIX "")
set_target_properties(imgui_ext PROPERTIES SUFFIX ".cse")

set_target_properties(imgui_legacy_ext PROPERTIES OUTPUT_NAME imgui_legacy)
set_target_properties(imgui_legacy_ext PROPERTIES PREFIX "")
set_target_properties(imgui_legacy_ext PROPERTIES SUFFIX ".cse")