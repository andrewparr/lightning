cmake_minimum_required(VERSION 3.0)
project(lightning)

set(SOURCES
    main.cpp
    maze.h
    maze.cpp
)

set(LIB_DEPS
    ncurses
)

add_executable(lightning ${SOURCES} )
target_link_libraries(lightning ${LIB_DEPS})
set_property(TARGET lightning PROPERTY CXX_STANDARD 20)