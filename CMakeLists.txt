cmake_minimum_required(VERSION 3.16.2)

set(CMAKE_CXX_STANDARD 17)

set(projectname "RacingSimulator")

project(${projectname})

file( GLOB GAME
        src/*.c
        src/*.cpp
        src/*.cc
        src/*.h
        src/*.hpp
)

#file (
#        GLOB _Willem
#        src/Willem/*.cpp
#        src/Willem/*.h
#)
file (
        GLOB _Sam
        src/Sam/*.cpp
        src/Sam/*.h
)
#file (
#        GLOB _Mohammed
#        src/Mohammed/*.cpp
#        src/Mohammed/*.h
#)
#file (
#        GLOB _Said
#        src/Said/*.cpp
#        src/Said/*.h
#)

#add_library(
#        Mohammed
#        ${_Mohammed}
#)
#add_library(
#        Willem
#        ${_Willem}
#)
add_library(
        Sam
        ${_Sam}
)
#add_library(
#        Said
#        ${_Said}
#)

add_executable(

        ${projectname}
        ${GAME}

)

target_link_libraries(${projectname}
#        Said
        Sam
#        Willem
#        Mohammed
)