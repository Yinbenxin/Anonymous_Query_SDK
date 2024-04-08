cmake_minimum_required(VERSION 3.12)

project(faker)

set(CMAKE_CXX_STANDARD 11)


# 添加头文件路径
include_directories(include)

# 查找并链接gtest库
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

set(SOURCES
        src/Faker.cpp
        src/Util.cpp
        )
# 生成库
add_library(faker ${SOURCES})

add_executable(faker_main  main.cpp)
target_link_libraries(faker_main faker)

#add_executable(faker_test tests/FakerTest.cpp)
#target_link_libraries(faker_test ${GTEST_LIBRARIES} faker pthread)
#
#add_executable(faker_mult_thread_test tests/FakerMultThreadTest.cpp)
#target_link_libraries(faker_mult_thread_test ${GTEST_LIBRARIES} faker pthread)