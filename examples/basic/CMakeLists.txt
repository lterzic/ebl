foreach(mode precise fast)
    add_executable(ebl-example-${mode} basic.c matrix.cpp)
    target_link_libraries(ebl-example-${mode} PRIVATE ebl-main ebl-stdio ebl-measure-${mode})
endforeach()
