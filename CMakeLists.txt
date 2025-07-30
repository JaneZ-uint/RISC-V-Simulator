cmake_minimum_required(VERSION 3.10)
project(RISCVSim)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${PROJECT_SOURCE_DIR}/src)

set(SOURCES
    main.cpp
)

add_executable(code ${SOURCES})
