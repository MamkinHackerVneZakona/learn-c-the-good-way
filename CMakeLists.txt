cmake_minimum_required(VERSION 3.12)
project(learn_c_the_good_way)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Release CACHE STRING "Type of build" FORCE)
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/bin CACHE STRING "Type of build" FORCE)
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    Release)

include_directories(include)

find_package(glfw3 3.3 QUIET PATHS ${PROJECT_SOURCE_DIR}/libs)
find_package(glfw3 3.3 QUIET) #imports glfw target
find_package(Freetype QUIET PATHS ${PROJECT_SOURCE_DIR}/libs NO_DEFAULT_PATH)
find_package(Freetype QUIET)

if(NOT ${FREETYPE_FOUND})
    message( SEND_ERROR "Freetype not found" )
else()
    message( STATUS "Freetype found: " ${FREETYPE_LIBRARIES} )  
endif()

if(NOT ${glfw3_FOUND} )
    message( SEND_ERROR "glfw3 not found" )
else()
    message( STATUS "glfw3 found: " ${glfw3_DIR})
endif()   

include_directories(${FREETYPE_INCLUDE_DIRS})

add_executable(learn_c_the_good_way src/glad.c src/main.cpp include/vec.h include/util.h include/Line.h include/render.h src/render.cpp include/renderImpl.h src/renderImpl.cpp src/util.cpp src/vec.cpp)

target_link_libraries(learn_c_the_good_way glfw ${FREETYPE_LIBRARIES})

install(TARGETS learn_c_the_good_way
        CONFIGURATIONS Release
        RUNTIME DESTINATION Release)