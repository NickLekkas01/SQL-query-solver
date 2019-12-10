cmake_minimum_required(VERSION 3.15)
project(tempproj)

set(CMAKE_CXX_STANDARD 14)

add_executable(tempproj main.cpp DBRelationsReader.cpp DBRelationsReader.h QueryList.cpp QueryList.h QueryEditor.cpp QueryEditor.h)