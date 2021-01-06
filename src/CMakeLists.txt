# CMakeList.txt : CMake project for JSynth, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)


project(q_example)
# Add source to this project's executable.
add_executable (JSynth "JSynth.cpp")
target_link_libraries(JSynth libq libqio cycfi::infra)

# TODO: Add tests and install targets if needed.