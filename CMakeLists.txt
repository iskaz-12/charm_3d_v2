cmake_minimum_required(VERSION 3.4)
project(charm_3d_v2)

find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    # Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/contrib/yaml-cpp/CMakeLists.txt")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

add_subdirectory(contrib/yaml-cpp)


set(CMAKE_CXX_STANDARD 14)


set(CHARM_INCLUDE
        "${CMAKE_SOURCE_DIR}/src"
        "${CMAKE_SOURCE_DIR}/src/mesh"
        "${CMAKE_SOURCE_DIR}/src/mesh/readers"
        "${CMAKE_SOURCE_DIR}/src/method"
        "${CMAKE_SOURCE_DIR}/src/method/fvm"
        )

include_directories(${CHARM_INCLUDE})



add_executable( charm_3d_v2
        src/main.cpp
        src/mesh/Mesh.cpp
        src/mesh/Mesh.h
        src/mesh/Cell.cpp
        src/mesh/Cell.h
        src/global.h
        src/global.cpp
        src/mesh/Point.cpp
        src/mesh/Point.h
        src/mesh/Face.cpp
        src/mesh/Face.h
        src/mesh/readers/MeshReader.cpp
        src/mesh/readers/MeshReader.h
        src/mesh/readers/MeshReaderMsh22ASCII.cpp
        src/mesh/readers/MeshReaderMsh22ASCII.h
        src/method/Config.cpp src/method/Config.h src/method/Material.cpp src/method/Material.h src/method/Region.cpp src/method/Region.h src/method/Component.cpp src/method/Component.h src/method/Method.cpp src/method/Method.h src/method/fvm/MethodFvm.cpp src/method/fvm/MethodFvm.h src/method/fvm/ConfigFvm.cpp src/method/fvm/ConfigFvm.h src/method/Reaction.cpp src/method/Reaction.h src/method/BoundaryCondition.cpp src/method/BoundaryCondition.h src/method/MethodException.cpp src/method/MethodException.h src/method/fvm/MethodFvmException.cpp src/method/fvm/MethodFvmException.h src/method/ConfigException.cpp src/method/ConfigException.h src/method/Data.cpp src/method/Data.h src/method/Flux.cpp src/method/Flux.h src/method/FluxHllc.cpp src/method/FluxHllc.h src/method/FluxLf.cpp src/method/FluxLf.h src/method/EosException.cpp src/method/EosException.h)

target_link_libraries(charm_3d_v2 yaml-cpp)