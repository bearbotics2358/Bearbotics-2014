cmake_minimum_required(VERSION 2.8)
include(FRCDeploy)

project(Bearbotics2014)
find_package(WPILib)
include_directories(${WPILib_INCLUDE_DIRS})

file(GLOB_RECURSE SOURCE_CODE *.cpp)

add_executable(bearbotics-2014 ${SOURCE_CODE})
target_link_libraries(bearbotics-2014 ${WPILib_LIBRARY})

add_make_deploy(bearbotics-2014 10.23.58.2)
