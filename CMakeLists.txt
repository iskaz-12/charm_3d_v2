cmake_minimum_required(VERSION 3.4)
project(charm_3d_v2)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
    message(STATUS "Build type not specified: Use Release by default")
endif(NOT CMAKE_BUILD_TYPE)




set(CONTRIB_DIR "${PROJECT_SOURCE_DIR}/contrib")

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

if(NOT EXISTS "${CONTRIB_DIR}/yaml-cpp/CMakeLists.txt")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

add_subdirectory(${CONTRIB_DIR}/yaml-cpp)
add_subdirectory(${CONTRIB_DIR}/metis-5.1.0)


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_COMPILER mpicc)
set(CMAKE_CXX_COMPILER mpicxx)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2 -DNDEBUG") # for production
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -DCHARM_DEBUG") # for developing
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -DNDEBUG") # for production
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -DCHARM_DEBUG") # for developing

set(METIS_INCLUDE "${CONTRIB_DIR}/metis-5.1.0/include")
set(CHARM_INCLUDE
        "${CMAKE_SOURCE_DIR}/src"
        "${CMAKE_SOURCE_DIR}/src/mesh"
        "${CMAKE_SOURCE_DIR}/src/mesh/readers"
        "${CMAKE_SOURCE_DIR}/src/method"
        "${CMAKE_SOURCE_DIR}/src/method/fvm"
        "${CMAKE_SOURCE_DIR}/src/method/lmchem"
        )

include_directories(${METIS_INCLUDE} ${CHARM_INCLUDE})



add_executable( charm_3d_v2
        src/main.cpp
        src/global.h
        src/global.cpp
        src/Operators.cpp
        src/Operators.h
        src/Parallel.cpp
        src/Parallel.h
        src/Log.cpp
        src/Log.h

        src/mesh/Mesh.cpp
        src/mesh/Mesh.h
        src/mesh/Cell.cpp
        src/mesh/Cell.h
        src/mesh/Point.h
        src/mesh/Face.cpp
        src/mesh/Face.h
        src/mesh/MeshException.h
        src/mesh/readers/MeshReaderException.h
        src/mesh/readers/MeshDecompException.h
        src/mesh/readers/MeshReader.cpp
        src/mesh/readers/MeshReader.h
        src/mesh/readers/MeshReaderMsh22ASCII.cpp
        src/mesh/readers/MeshReaderMsh22ASCII.h

        src/method/Config.cpp
        src/method/Config.h
        src/method/Material.cpp
        src/method/Material.h
        src/method/Region.cpp
        src/method/Region.h
        src/method/Component.cpp
        src/method/Component.h
        src/method/Method.h
        src/method/Reaction.cpp
        src/method/Reaction.h
        src/method/BoundaryCondition.cpp
        src/method/BoundaryCondition.h
        src/method/MethodException.cpp
        src/method/MethodException.h
        src/method/ConfigException.cpp
        src/method/ConfigException.h
        src/method/EosException.h
        src/method/VtkWriter.cpp
        src/method/VtkWriter.h
        src/method/Prim.h
        src/method/Prim.cpp

        src/method/fvm/MethodFvm.cpp
        src/method/fvm/MethodFvm.h
        src/method/fvm/ConfigFvm.cpp
        src/method/fvm/ConfigFvm.h
        src/method/fvm/FluxFvm.cpp
        src/method/fvm/FluxFvm.h
        src/method/fvm/FluxFvmHllc.cpp
        src/method/fvm/FluxFvmHllc.h
        src/method/fvm/MethodFvmException.cpp
        src/method/fvm/MethodFvmException.h

        src/method/lmchem/MethodFvmLMCh.h
        src/method/lmchem/MethodFvmLMCh.cpp
        src/method/lmchem/MethodFvmLMChException.h
        src/method/lmchem/ConfigFvmLMCh.cpp
        src/method/lmchem/ConfigFvmLMCh.h
        src/method/lmchem/FluxFvmLMCh.h
        src/method/lmchem/FluxFvmLMChLF.cpp
        src/method/lmchem/FluxFvmLMChLF.h
        src/method/lmchem/MethodFvmLMCh_chem.cpp
        src/method/lmchem/MethodFvmLMCh_adv.cpp
        src/method/lmchem/MethodFvmLMCh_visc.cpp
        src/method/lmchem/MethodFvmLMCh_diff.cpp
        src/method/lmchem/MethodFvmLMCh_press.cpp
        src/method/lmchem/MethodFvmLMCh_grad.cpp
        src/method/lmchem/MethodFvmLMCh_heat.cpp
        src/method/lmchem/MethodFvmLMCh_conv.cpp


)

target_link_libraries(charm_3d_v2 yaml-cpp metis)
add_dependencies(charm_3d_v2  yaml-cpp metis)