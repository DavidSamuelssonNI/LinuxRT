cmake_minimum_required(VERSION 3.10)
project(IDLParserExample)

set(CMAKE_CXX_STANDARD 11)

add_executable(IDLParserExample idl_parser.cpp)
add_executable(generate_listener generate_listener.cpp)
add_executable(generate_header generate_header_for_LV.cpp)
add_executable(cppmodelparser parse_header_to_parameters_for_vs_cppmodel.cpp)

# Optionally, you can include this if your compiler supports filesystem
# target_compile_features(IDLParserExample PRIVATE cxx_std_17)

# Uncomment the following line if you want to build with C++17 and have filesystem support
# target_compile_features(IDLParserExample PRIVATE cxx_std_17)

# If your compiler doesn't support filesystem, you may need to link to the filesystem library
# target_link_libraries(IDLParserExample stdc++fs)
