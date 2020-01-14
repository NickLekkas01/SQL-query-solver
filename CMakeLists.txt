cmake_minimum_required(VERSION 3.15)
project(tempproj)

set(CMAKE_CXX_STANDARD 14)

add_executable(tempproj main.cpp Utilities.cpp Utilities.h QuickSort.cpp QuickSort.h List.cpp List.h DBRelationsReader.cpp DBRelationsReader.h QueryList.cpp QueryList.h QueryEditor.cpp QueryEditor.h UtilQE.cpp UtilQE.h Hashtable.cpp Hashtable.h Perm.cpp Perm.h)