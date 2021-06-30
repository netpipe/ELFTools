cmake_minimum_required(VERSION 3.10)

project(elfcat VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

configure_file(config.h.in config.h)

add_subdirectory(src/utils)
add_subdirectory(src/elf)

#set(cxx_library "stdc++fs")
#add_library(stdc++fs)
#add_library( stdc++fs SHARED IMPORTED )
#TARGET_LINK_LIBRARIES(stdc++fs)

add_executable(elfcat
    src/main.cpp
    src/report_gen.cpp
)

target_link_libraries(elfcat PUBLIC
    elf
    utils
stdc++fs
)

target_include_directories(elfcat PUBLIC
    "${PROJECT_BINARY_DIR}" 
)

add_subdirectory(example)

install(TARGETS elfcat
        RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin
)

install(DIRECTORY src/data DESTINATION ${CMAKE_SOURCE_DIR}/bin
)
