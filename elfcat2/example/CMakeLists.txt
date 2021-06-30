project(example VERSION 1.0)

add_executable(example
    example.cpp
)

install(TARGETS example
        RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin
)
