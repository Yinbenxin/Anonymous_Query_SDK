一、说明

为了更加方便的支持跨语言编译，提供了python封装方式，通过cmakelist进行编译，会自动生成相关lib及其他依赖，比如py会生成seal.so。

二、需要依赖库

1. swig

   ```bash
   git clone https://github.com/pybind/pybind11.git
   cd pybind11
   cmake -B build
   cmake --build build
   cmake --install build
   ```


