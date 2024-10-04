cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 20)

file(GLOB solver_ppc
     "*.cpp")

add_executable(main ${solver_ppc})

target_include_directories(main
 PRIVATE
 ${solver_ppc_SOURCE_DIR}/include)