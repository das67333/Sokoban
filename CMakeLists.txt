cmake_minimum_required(VERSION 3.17)
project(sokoban)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -O2")

# Enable debug symbols by default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug 
    CACHE STRING 
      "Choose the type of build (Debug or Release)" 
      FORCE
    )
endif()

# Find SFML shared libraries
find_package(SFML 2.5 
  COMPONENTS 
    system window graphics network audio REQUIRED
)

# Compile executable
add_executable(sokoban
    src/main.cpp
    src/gamelogic.cpp
    src/graphics.cpp
    src/tinyxml2.cpp
)

# Set include directory search paths
target_include_directories(sokoban 
  PRIVATE
  "${PROJECT_BINARY_DIR}"
  )

# Link executable to required SFML libraries
target_link_libraries(sokoban sfml-graphics)

# Install target
install(TARGETS sokoban DESTINATION bin)
