# CMake entry point
cmake_minimum_required (VERSION 3.0)
project (AdvancedAnimation)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")


include_directories(
    /usr/local/include/eigen3/
    ${OPENGL_INCLUDE_DIRS}
    ${GLUT_INCLUDE_DIRS}
)

add_executable(main
src/main.cpp
src/camera.cpp
src/tree.cpp
src/node.cpp
src/joint.cpp
src/solver.cpp
)

target_link_libraries(main
    ${OPENGL_LIBRARY}
    ${GLUT_LIBRARY}
)
