project(ncc)

#版本检查
cmake_minimum_required(VERSION 2.8)

#头文件目录

#源文件目录

set(CMAKE_C_FLAGS -g)
set(LIBRARY_OUTPUT_PATH lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ./)
set(EXECUTABLE_OUTPUT_PATH ..)

#add_library(rs SHARED src/reed-solomon.c)
#add_library(rs-static SHARED src/reed-solomon.c)


#添加可执行程序
add_executable(ncc  ncc.c)

