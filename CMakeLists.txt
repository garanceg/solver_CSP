cmake_minimum_required(VERSION 3.1)
project(my_project)

add_executable(my_executable
    main.cpp
)

target_compile_features(my_executable PUBLIC cxx_std_17)