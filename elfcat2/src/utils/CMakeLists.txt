add_library(utils OBJECT
    utils.cpp
)

target_include_directories(utils
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include
    PUBLIC ${CMAKE_SOURCE_DIR}/src/utils/include
)

# install(FILES include/utils.hpp DESTINATION ${CMAKE_SOURCE_DIR}/include/utils)
