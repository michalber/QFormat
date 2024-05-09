cmake_minimum_required(VERSION 3.23)

project(
    QFormat
    VERSION 1.0.0
    DESCRIPTION "C++17 template fixed point handler"
    LANGUAGES CXX
)

option(QFORMAT_COMPILE "Build as a static/shared library" OFF)
option(QFORMAT_USE_FLOAT_VAL "Include additional float member to class" OFF)

include(cmake/CompileOptions.cmake)

if(QFORMAT_COMPILE)
    set(SCOPE PUBLIC)
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/qformat.cpp "#include <qformat.hpp>")
    add_library(qformat ${CMAKE_CURRENT_BINARY_DIR}/singleton.cpp)
else()
    set(SCOPE INTERFACE)
    add_library(qformat INTERFACE)
endif()

# Create a qformat target
add_library(qformat::qformat ALIAS qformat)
target_compile_features(qformat ${SCOPE} cxx_std_17)
target_compile_definitions(qformat ${SCOPE}
    $<$<BOOL:${QFORMAT_USE_FLOAT_VAL}>:QFORMAT_USE_FLOAT_VAL>
)
target_sources(qformat ${SCOPE}
    FILE_SET HEADERS
    BASE_DIRS include
    FILES include/qformat.hpp
)

if(PROJECT_IS_TOP_LEVEL AND NOT CMAKE_CROSSCOMPILING)
    include(CTest)
    add_subdirectory(test)
endif()