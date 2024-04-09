project(PIR_LIBS)
cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
# 查找并链接gtest库
set(IS_TEST )
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
include_directories(
        pir/include
        pir/thirdparty
        faker/include
#        interface
)

add_subdirectory(faker)
add_subdirectory(pir)
add_subdirectory(swig-go)
add_subdirectory(swig-java)
add_subdirectory(swig-python)

set(LIBNAME "pirface")

if (APPLE AND CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    set(LIBNAME "pirface_x64")
endif()


#add_library(${LIBNAME} SHARED interface/interface.cpp interface/interface.h)
#pir链接依赖
#target_link_libraries(${LIBNAME} pir faker)
#add_executable(interface main.cpp )
#target_link_libraries(interface ${LIBNAME})
