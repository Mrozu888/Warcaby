cmake_minimum_required(VERSION 3.10)
project(Warcaby C)

set(CMAKE_C_STANDARD 11)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(Warcaby main.c game_logic.c ui.c bot.c stats.c)
target_link_libraries(Warcaby ${CURSES_LIBRARIES})
