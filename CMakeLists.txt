cmake_minimum_required(VERSION 3.10)  
  
project(kPlexS)  
  
set(CMAKE_CXX_STANDARD 17)  
set(CMAKE_CXX_STANDARD_REQUIRED ON)  
  
set(SOURCE_FILES main.cpp Graph.cpp)  
  
add_executable(kPlexS ${SOURCE_FILES})  

target_compile_options(kPlexS PRIVATE -flto -O3)  
target_include_directories(kPlexS PRIVATE .)  
  
# 清理构建文件  
add_custom_target(clean_build ALL  
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/.obj  
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/.obj  
)