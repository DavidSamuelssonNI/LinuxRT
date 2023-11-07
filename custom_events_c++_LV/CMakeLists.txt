cmake_minimum_required(VERSION 3.12)
project(SineWaveGenerator)

set(CMAKE_CXX_STANDARD 11)

# Define the shared library
# add_library(sine_wave SHARED sinus_wave.cpp)

# Add the main executable
add_library(mylib SHARED mylib.cpp)
# add_executable(main main.cpp)

# Link the main executable with the shared library
target_link_libraries( mylib dl)

# Include the directory where the header file is located
#target_include_directories(main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
# target_include_directories(sine_wave PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
