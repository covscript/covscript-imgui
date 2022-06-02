cmake_minimum_required(VERSION 3.4)

project(covscript-imgui)
include_directories(backends)
include_directories(include)
include_directories(include/imgui)

if (DEFINED ENV{CS_DEV_PATH})
    include_directories($ENV{CS_DEV_PATH}/include)
    link_directories($ENV{CS_DEV_PATH}/lib)
endif ()

if (DEFINED ENV{CS_DEV_OUTPUT})
    set(LIBRARY_OUTPUT_PATH $ENV{CS_DEV_OUTPUT})
    set(EXECUTABLE_OUTPUT_PATH $ENV{CS_DEV_OUTPUT})
endif ()

# Compiler Options
set(CMAKE_CXX_STANDARD 14)

if (MSVC)
    set(CMAKE_CXX_FLAGS "/O2 /EHsc /utf-8 /w")
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
elseif (CMAKE_COMPILER_IS_GNUCXX)
    if (WIN32)
        set(CMAKE_C_FLAGS "--static -fPIC -s -O3")
        set(CMAKE_CXX_FLAGS "--static -fPIC -s -O3")
    else ()
        set(CMAKE_C_FLAGS "-fPIC -s -O3")
        set(CMAKE_CXX_FLAGS "-fPIC -s -O3")
    endif ()
else ()
    set(CMAKE_C_FLAGS "-fPIC -O3")
    set(CMAKE_CXX_FLAGS "-fPIC -O3")
endif ()

if (WIN32)
set(IMGUI_SOURCE_CODE
        backends/imgui_impl_win32.cpp
        res/default_font.cpp
        src/gl3w.c
        src/imgui.cpp
        src/imgui_demo.cpp
        src/imgui_draw.cpp
        src/imgui_widgets.cpp
        src/imgui_tables.cpp)
else ()
set(IMGUI_SOURCE_CODE
        backends/imgui_impl_glfw.cpp
        res/default_font.cpp
        src/gl3w.c
        src/imgui.cpp
        src/imgui_demo.cpp
        src/imgui_draw.cpp
        src/imgui_widgets.cpp
        src/imgui_tables.cpp)
endif ()

add_library(imgui STATIC ${IMGUI_SOURCE_CODE})
add_library(imgui_font_ext SHARED imgui_font.cpp)
if (WIN32)
    add_library(imgui_ext SHARED backends/imgui_impl_dx9.cpp imgui.cpp)
else ()
    #add_library(imgui_gl2_ext SHARED backends/imgui_impl_opengl2.cpp imgui.cpp)
    add_library(imgui_ext SHARED backends/imgui_impl_opengl3.cpp imgui.cpp)
    target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GL3W)
    target_compile_definitions(imgui_gl2_ext PRIVATE IMGUI_IMPL_GL2)
endif ()

if (WIN32)
    target_link_libraries(imgui d3d9 d3dx9 gdi32 opengl32 imm32 dwmapi)
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

set_target_properties(imgui_font_ext PROPERTIES OUTPUT_NAME imgui_font)
set_target_properties(imgui_font_ext PROPERTIES PREFIX "")
set_target_properties(imgui_font_ext PROPERTIES SUFFIX ".cse")

set_target_properties(imgui_ext PROPERTIES OUTPUT_NAME imgui)
set_target_properties(imgui_ext PROPERTIES PREFIX "")
set_target_properties(imgui_ext PROPERTIES SUFFIX ".cse")