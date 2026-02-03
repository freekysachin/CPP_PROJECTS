cmake_minimum_required(VERSION 3.20.1)
project(threadPool)

file(GLOB SOURCES
    src/*.cpp
)
add_executable(threadPool
    main.cpp
    ${SOURCES}
)

target_include_directories(threadPool
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(threadPool PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
)

# Automatically copy .env to the directory containing the executable after every build.
add_custom_command(TARGET threadPool POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/.env
        $<TARGET_FILE_DIR:threadPool>
    COMMENT "Copying .env to executable directory..."
)
