cmake_minimum_required(VERSION 3.16.3)
set(CMAKE_CXX_FLAGS "-pthread")
set(CMAKE_CXX_STANDARD 20)
set(APP_EXE RacingSimulator)

project(${APP_EXE})

file(GLOB_RECURSE source_ai
        src/Ai/*.cpp
        src/Ai/*.h
        )

# Automata and grammars
file(GLOB_RECURSE source_automata_and_grammars
        src/AutomataAndGrammars/*.cpp
        src/AutomataAndGrammars/*.h
        )

if (WIN32)
    # Image parser
    file(GLOB_RECURSE imageclasses
            src/imageclasses/*.cpp
            src/imageclasses/*.h
            )
endif ()

# Artificial intelligence
file(GLOB_RECURSE source_ai
        src/Ai/*.cpp
        src/Ai/*.h
        )

# Game representation
file(GLOB_RECURSE source_game_representation
        src/main.cpp
        src/game_representation/*.cpp
        src/game_representation/*.h
        )

if (WIN32)
    add_executable(${APP_EXE} ${source_game_representation} ${source_automata_and_grammars} ${source_ai} ${imageclasses})
else ()
    add_executable(${APP_EXE} ${source_game_representation} ${source_automata_and_grammars} ${source_ai}) #${imageclasses})
endif ()

# Game logic static library
file(GLOB_RECURSE source_game_logic
        src/game_logic/*.cpp
        src/game_logic/*.h
        )

ADD_LIBRARY(SOURCE_GAME_LOGIC_LIB STATIC ${source_game_logic})

# OpenCV
if (WIN32)
    link_directories("${CMAKE_CURRENT_SOURCE_DIR}/libs/opencv-4.x/build/bin/")
    find_package(OpenCV REQUIRED)
    include_directories(${OpenCV_INCLUDE_DIRS})

    TARGET_LINK_LIBRARIES(${APP_EXE} SOURCE_GAME_LOGIC_LIB ${OpenCV_LIBS})
else ()
    TARGET_LINK_LIBRARIES(${APP_EXE} SOURCE_GAME_LOGIC_LIB)
endif ()

# (Windows) Path to SFML library
if (WIN32)
    set(SFML_ROOT C:\\Users\\samro\\Documents\\School\\2021-2022\\AP\\AP_Project_2021-2022\\SFML-2.5.1_WIN\\)
endif ()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")
find_package(SFML REQUIRED system window graphics network audio)
if (SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(${APP_EXE} ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
endif ()

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

# (Windows) Path to mingw64/bin
if (WIN32)
    #    set(MINGW_ROOT "C:\\Program Files\\Mingw\\mingw64\\bin")
    #    set(MINGW_ROOT C:/mingw64/bin/)
endif ()

# (Windows) Copy dlls to build
if (WIN32)
    # sfml
    file(GLOB BINARY_DEP_DLLS "${SFML_INCLUDE_DIR}/../bin/*.dll")
    file(COPY ${BINARY_DEP_DLLS} DESTINATION ${CMAKE_BINARY_DIR})
    # mingw
    file(GLOB MINGW_DEP_DLLS "${MINGW_ROOT}/*.dll")
    file(COPY ${MINGW_DEP_DLLS} DESTINATION ${CMAKE_BINARY_DIR})
    #opencv
    file(GLOB DLLS "${CMAKE_CURRENT_LIST_DIR}/libs/opencv-4.x/build/bin/*.dll")
    file(COPY ${DLLS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif ()

