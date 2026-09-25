cmake_minimum_required(VERSION 3.10)
project(FileSearch)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(MSVC)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

add_subdirectory(external/glfw)

# -------- GLAD --------
add_library(glad STATIC
    external/glad/src/glad.c
)
target_include_directories(glad PUBLIC
    ${CMAKE_SOURCE_DIR}/external/glad/include
)

# -------- Your Code --------
file(GLOB_RECURSE FILESEARCH_SOURCES
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)
file(GLOB_RECURSE FILESEARCH_HEADERS
    "${CMAKE_SOURCE_DIR}/inc/*.h"
    "${CMAKE_SOURCE_DIR}/inc/*.hpp"
)
# -------- ImGui --------
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/external/imgui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/imgui_impl_opengl3.cpp
    # ${IMGUI_DIR}/implot.cpp
    # ${IMGUI_DIR}/implot_items.cpp
    # ${IMGUI_DIR}/implot_demo.cpp
)

add_executable(FileSearch
    ${FILESEARCH_SOURCES}
    ${FILESEARCH_HEADERS}
    ${IMGUI_SOURCES}
)

target_include_directories(FileSearch PRIVATE
    ${CMAKE_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/external/glad/include
    ${CMAKE_SOURCE_DIR}/external/glfw/include
    ${IMGUI_DIR}
)

target_link_libraries(FileSearch PRIVATE
    glad
    glfw
    opengl32
)

if(WIN32)
    add_custom_command(TARGET FileSearch POST_BUILD
        # Copy the GLFW DLL to the output directory
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:glfw>
            $<TARGET_FILE_DIR:FileSearch>)
endif()
