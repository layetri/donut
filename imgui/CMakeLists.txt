cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 20)

add_library(
        imgui STATIC
        imconfig.h
        imgui.cpp
        imgui.h
        imgui_demo.cpp
        imgui_draw.cpp
        imgui_tables.cpp
        imgui_widgets.cpp
        backends/imgui_impl_glfw.cpp
        backends/imgui_impl_glfw.h
        backends/imgui_impl_opengl3.h
        backends/imgui_impl_opengl3_loader.h
        backends/imgui_impl_opengl3.cpp
        backends/imgui_impl_opengl2.h
        backends/imgui_impl_opengl2.cpp
        imgui_internal.h
        imstb_rectpack.h
        imstb_textedit.h
        imstb_truetype.h
)

include_directories(PRIVATE "/usr/local/include")
include_directories(PRIVATE "/opt/homebrew/include")