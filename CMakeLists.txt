cmake_minimum_required(VERSION 3.5.1)

project(happy-network-train)
set(HAPPY_N_TRAIN_TARGET_VERSION "0.0.1" CACHE STRING "happy-network-train version")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_COMPILER_ID "Clang")
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wall -D__FILENAME__='\"$(subst ${CMAKE_SOURCE_DIR}/,,$(abspath $<))\"'")

aux_source_directory(src/ PROJECT_SRC)

add_executable(happyntrain main.cpp ${PROJECT_SRC})


