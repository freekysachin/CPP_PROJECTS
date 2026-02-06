cmake_minimum_required(VERSION 3.20.1)
project(multithreadedserver)

file(GLOB SOURCES
    src/*.cpp
)
add_executable(multithreadedserver
    main.cpp
    ${SOURCES}
)

target_include_directories(multithreadedserver
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/includes
)

set_target_properties(multithreadedserver PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
)