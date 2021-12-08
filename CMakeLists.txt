cmake_minimum_required(VERSION 3.16.2)

set(CMAKE_CXX_STANDARD 14)

set(projectname "RacingSimulator")

project(${projectname})

file( GLOB GAME
        src/*.c
        src/*.cpp
        src/*.cc
        src/*.h
        src/*.hpp
)

add_executable(
        ${projectname}
        ${GAME}
)