cmake_minimum_required(VERSION 3.23)

project(fs_ext LANGUAGES CXX)
add_library(fs_ext src/temporary_directory.cpp)
target_include_directories(fs_ext PUBLIC include)
add_library(fs_ext::fs_ext ALIAS fs_ext)
target_compile_features(fs_ext PUBLIC cxx_std_20)
