cmake_minimum_required(VERSION 3.17)
project(charm_3d_v2)

set(CMAKE_CXX_STANDARD 14)


set(CHARM_INCLUDE "${CMAKE_SOURCE_DIR}/src" "${CMAKE_SOURCE_DIR}/src/mesh")

include_directories(${CHARM_INCLUDE})



add_executable( charm_3d_v2
        src/main.cpp
        src/mesh/Mesh.cpp
        src/mesh/Mesh.h
        src/mesh/Cell.cpp
        src/mesh/Cell.h
        src/global.h
        src/mesh/Point.cpp
        src/mesh/Point.h
        src/mesh/Face.cpp src/mesh/Face.h)