cmake_minimum_required(VERSION 3.1)

project(satallite)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmakeModules")

find_package(OpenGL REQUIRED)
add_definitions(${OPENGL_DEFINITIONS})
find_package(X11 REQUIRED)
find_package(GLFW3 REQUIRED)

add_library(GLAD lib/glad.c)
add_library(STB_IMAGE lib/stb_image.cpp)
set(LIBS ${GLFW3_LIBRARY} X11 Xrandr Xinerama Xi Xcursor GL dl pthread ${ASSIMP_LIBRARY})

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} GLAD)
target_link_libraries(${PROJECT_NAME} STB_IMAGE)
target_link_libraries(${PROJECT_NAME} ${LIBS})
include_directories(include)