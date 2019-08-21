cmake_minimum_required(VERSION 3.12)
project(learn_c_the_good_way)

set(CMAKE_CXX_STANDARD 14)

include_directories(glfw-3.2.1/include)
include_directories(include)

find_package(glfw3 3.3 REQUIRED)
find_package(Freetype REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})

add_executable(learn_c_the_good_way src/glad.c src/main.cpp include/vec.h include/util.h include/Line.h include/render.h src/render.cpp include/renderImpl.h src/renderImpl.cpp src/util.cpp src/vec.cpp)

target_link_libraries(learn_c_the_good_way glfw ${FREETYPE_LIBRARIES})