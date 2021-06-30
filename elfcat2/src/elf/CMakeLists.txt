add_library(elf OBJECT
    defs.cpp
    elf32.cpp
    elf64.cpp
    parser.cpp
)

target_include_directories(elf
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(elf PRIVATE utils)
