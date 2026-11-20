cmake_minimum_required(VERSION 3.20)
project(e1)
set(CMAKE_CXX_STANDARD 17)
add_executable(e1 src/main.cpp src/my_scheduler.cpp src/simulator.cpp)