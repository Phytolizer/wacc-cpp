cmake_minimum_required(VERSION 3.23)

project(wacc LANGUAGES CXX)

add_definitions(-DANTLR4CPP_STATIC)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(ANTLR REQUIRED)

include(ExternalAntlr4Cpp)
include_directories(${ANTLR4_INCLUDE_DIRS})

include(FetchContent)
FetchContent_Declare(
  cli11
  GIT_REPOSITORY https://github.com/CLIUtils/CLI11
  GIT_TAG v2.2.0
)
FetchContent_MakeAvailable(cli11)

FetchContent_Declare(
  rang
  GIT_REPOSITORY https://github.com/agauniyal/rang
  GIT_TAG v3.2
)
FetchContent_MakeAvailable(rang)

antlr_target(
  WaccParser ${CMAKE_CURRENT_SOURCE_DIR}/src/wacc/C.g4 LEXER PARSER LISTENER
  PACKAGE wacc
  OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/antlr4_wacc
)
add_library(
  wacc ${ANTLR_WaccParser_CXX_OUTPUTS} src/wacc/run.cc src/wacc/codegen.cc
)
target_include_directories(
  wacc
  PRIVATE ${ANTLR_WaccParser_OUTPUT_DIR} src/wacc/include
  PUBLIC include
)
target_compile_features(wacc PUBLIC cxx_std_20)
target_link_libraries(wacc PRIVATE antlr4_static CLI11::CLI11 rang)

add_executable(wacc_driver src/wacc_driver/main.cc)
target_link_libraries(wacc_driver PRIVATE wacc)
