cmake_minimum_required(VERSION 3.10)

# set the project name
project(LDtkD11)

find_package(directxtk CONFIG REQUIRED)

# add the executable
add_executable(LDtkD11 Game.cpp Main.cpp pch.cpp)

target_link_libraries(main PRIVATE Microsoft::DirectXTK)