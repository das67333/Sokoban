cmake_minimum_required(VERSION 3.17)
project(sokoban)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -O2")

find_package(SFML 2.5 
  COMPONENTS 
    system window graphics network audio REQUIRED
)

add_executable(sokoban
    src/main.cpp
    src/gamelogic.cpp
    src/graphics.cpp
    src/tinyxml2.cpp
)

target_link_libraries(sokoban sfml-graphics)
