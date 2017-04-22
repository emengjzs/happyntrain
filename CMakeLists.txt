cmake_minimum_required(VERSION 3.5.1)

set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__FILENAME__='\"$(subst ${CMAKE_SOURCE_DIR}/,,$(abspath $<))\"'")

project(happy-network-train)
set(HAPPY_N_TRAIN_TARGET_VERSION "0.0.1" CACHE STRING "happy-network-train version")

aux_source_directory(src/ PROJECT_SRC)

add_executable(happyntrain main.cpp ${PROJECT_SRC})


