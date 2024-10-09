cmake_minimum_required(VERSION 3.12)

# Set C++ standard to 20
set(CMAKE_CXX_STANDARD 20)

# Find all .cpp files in the project
file(GLOB solver_ppc
     "src/*.cpp")  # Assuming your .cpp files are in the 'src' folder

# Create the executable
add_executable(main ${solver_ppc})

# Include header files from the 'include' directory
target_include_directories(main
 PRIVATE
 ${CMAKE_SOURCE_DIR}/include)