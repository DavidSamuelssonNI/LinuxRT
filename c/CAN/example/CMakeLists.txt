cmake_minimum_required(VERSION 3.0.0)
project(CAN_Frame_Stream_Input VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(CAN_Frame_Stream_Input CAN_Frame_Stream_Input_linux.c)
target_link_libraries(CAN_Frame_Stream_Input /usr/lib/x86_64-linux-gnu/libnixnet.so)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
#cmake -D CMAKE_CXX_COMPILER=x86_64-nilrt-linux-gcc .. && make && ./CAN_Frame_Stream_Input