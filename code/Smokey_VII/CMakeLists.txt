cmake_minimum_required(VERSION 2.8)
include(FRCDeploy)

project(Test)
find_package(WPILib)
include_directories(${WPILib_INCLUDE_DIRS})

file(GLOB_RECURSE SOURCE_CODE *.cpp)

add_executable(test-code ${SOURCE_CODE})
target_link_libraries(test-code ${WPILib_LIBRARY})

add_make_deploy(test-code 10.23.58.2)
