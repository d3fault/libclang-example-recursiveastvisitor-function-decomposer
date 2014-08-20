cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  "${CMAKE_SOURCE_DIR}/cmake/modules"
)

find_package(LibClang REQUIRED)

add_definitions(${LibClang_DEFINITIONS})

include_directories(
  ${LibClang_INCLUDE_DIRS}
)

add_executable(function-decomposer functiondecomposer.cpp)
target_link_libraries(function-decomposer /usr/lib/llvm-3.4/lib/libclangTooling.a)
target_link_libraries(function-decomposer /usr/lib/llvm-3.4/lib/libclangDriver.a)
target_link_libraries(function-decomposer /usr/lib/llvm-3.4/lib/libclangFrontend.a)
target_link_libraries(function-decomposer /usr/lib/llvm-3.4/lib/libclangParse.a)
target_link_libraries(function-decomposer /usr/lib/llvm-3.4/lib/libclangSema.a)
target_link_libraries(function-decomposer /usr/lib/llvm-3.4/lib/libclangEdit.a)
target_link_libraries(function-decomposer /usr/lib/llvm-3.4/lib/libclangAnalysis.a)
target_link_libraries(function-decomposer /usr/lib/llvm-3.4/lib/libclangLex.a)
target_link_libraries(function-decomposer /usr/lib/llvm-3.4/lib/libclangSerialization.a)
target_link_libraries(function-decomposer /usr/lib/llvm-3.4/lib/libclangAST.a)
target_link_libraries(function-decomposer /usr/lib/llvm-3.4/lib/libclangBasic.a)
target_link_libraries(function-decomposer /usr/lib/llvm-3.4/lib/libLLVM-3.4.so)
