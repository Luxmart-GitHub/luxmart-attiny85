cmake_minimum_required(VERSION 3.1)

project(luxmart-attiny85)

add_executable(${PROJECT_NAME}-test test/test.cpp)
target_include_directories(${PROJECT_NAME}-test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME}-test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test)
