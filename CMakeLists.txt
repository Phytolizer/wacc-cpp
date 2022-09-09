cmake_minimum_required(VERSION 3.23)

project(wacc LANGUAGES CXX)

add_definitions(-DANTLR4CPP_STATIC)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(ANTLR REQUIRED)

include(ExternalAntlr4Cpp)
include_directories(${ANTLR4_INCLUDE_DIRS})

antlr_target(WaccParser ${CMAKE_CURRENT_SOURCE_DIR}/src/wacc/C.g4 BOTH PACKAGE wacc OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/antlr4_wacc)
include_directories(${ANTLR_WaccParser_OUTPUT_DIR})
add_executable(wacc src/wacc/main.cc ${ANTLR_WaccParser_CXX_OUTPUTS})
target_compile_features(wacc PRIVATE cxx_std_20)
target_link_libraries(wacc PRIVATE antlr4_static)
