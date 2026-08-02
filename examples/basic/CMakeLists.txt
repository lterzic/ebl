add_executable(ebl-example basic.c matrix.cpp)
target_link_libraries(ebl-example PRIVATE ebl-main ebl-stdio)
