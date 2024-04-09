cmake_minimum_required(VERSION 3.10)
project(add)

set(CMAKE_CXX_STANDARD 11)

# 使用当前conda环境的python解释器
#find_package(PythonInterp)
#find_package(PythonLibs)
#set(PYTHON_EXECUTABLE /Library/Frameworks/Python.framework/Versions/3.11/bin/python3.11)
#message(PythonInterp)

find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11)
pybind11_add_module(seal seal.cpp)

set_target_properties(seal PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
target_link_libraries(seal pir faker)