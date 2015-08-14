cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(cubes CXX)

include_directories(src)

set(SRC_FILES 
	src/arrows.cpp
	src/board.cpp
	src/main.cpp
	src/mathematics.cpp
	src/move.cpp
	src/vec.cpp)

set(HEADER_FILES
	src/arrows.h
	src/board.h
	src/mathematics.h
	src/move.h
	src/vec.h)

add_executable(cubes ${SRC_FILES} ${HEADER_FILES})

# Add libraries
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )
target_link_libraries(cubes ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )

set(CMAKE_CXX_FLAGS "-O2")
