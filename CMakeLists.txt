cmake_minimum_required(VERSION 3.14)
project(selection)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

SET(GCC_LINK_FLAGS "-lz")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_LINK_FLAGS}")

include_directories(.)

add_executable(selection
        main.cpp
        MbRandom.cpp
        MbRandom.h
        gzstream.cpp
        gzstream.h
        mcmc.cpp
        mcmc.h
        measure.cpp
        measure.h
        param.cpp
        param.h
        path.cpp
        path.h
        popsize.cpp
        popsize.h
        settings.cpp
        settings.h)

find_package(GSL REQUIRED)
target_link_libraries(selection GSL::gsl GSL::gslcblas)