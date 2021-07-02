cmake_minimum_required(VERSION 3.10)

# set the project name
project(LDtkD11)

find_package(directxtk CONFIG REQUIRED)
find_package(directxmath CONFIG REQUIRED)


find_package(LDtkLoader)

# add the executable
add_executable(LDtkD11 WIN32 Game.cpp Main.cpp pch.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE
	d3d11.lib dxgi.lib dxguid.lib uuid.lib
    kernel32.lib user32.lib
    comdlg32.lib advapi32.lib shell32.lib
    ole32.lib oleaut32.lib
    Microsoft::DirectXTK
)

target_link_libraries(${PROJECT_NAME} PRIVATE Microsoft::DirectXMath)

target_link_libraries(${PROJECT_NAME} PRIVATE LDtkLoader)