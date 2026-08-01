cmake_minimum_required(VERSION 3.10)
project(ebl)

add_subdirectory(arch)

add_library(ebl-backend backend.c)
target_link_libraries(ebl-backend PRIVATE ebl-arch)