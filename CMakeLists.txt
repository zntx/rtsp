cmake_minimum_required(VERSION 3.10)

project(subprojects)

set(CMAKE_CXX_STANDARD 11)
include(dependencies.cmake) 
# Add sub directories
# 定义子目录src，用以递归的调用src中的MakeLists.txt
#add_subdirectory(3drpart)
include_directories(3drpart)
Depend(net git "https://github.com/zntx/net.git" )

Depend(fmt git "https://github.com/zntx/net.git" )

aux_source_directory(src SOURCE)
#set(SOURCE src/main.cpp)

add_executable( useHello ${SOURCE}  main.cpp)
target_link_libraries(useHello net fmt)




