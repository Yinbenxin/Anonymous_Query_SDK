一、说明

为了更加方便的支持跨语言编译，提供了swig封装方式，通过cmakelist进行编译，会自动生成相关lib及其他依赖，比如go会生成libpir-go-arm64.lib和pir.go。

二、需要依赖库

1. swig

   ```bash
   git clone https://github.com/swig/swig.git
   cd swig-go
   ./configure 
   make
   make -j8
   ```

三、test

   ```bash
   #编译依赖
   cd pir/cpp
   cmake -S . -B build
   cmake --build build
   #相关依赖会出现在cpp/cgo目录下。
   cp cgo/pir.go cgo/pir.go/libpir-go-arm64.dylib ../go/lib
   #进行测试
   cd ../go
   DYLD_LIBRARY_PATH="${your lib path}"; CGO_LDFLAGS="${your lib path} -lpir-go-arm64" go run main.go
   ```

四、go调用方法

   ```bash
   #将编译后的libpir-go-arm64.lib和pir.go从pir/cpp/cgo中移动到go工程中。
   #go文件import对应的文件，接下来设置lib路径。
   #若使用IDE工具可以设置环境变量。
   DYLD_LIBRARY_PATH=${your lib path};CGO_LDFLAGS=-L${your lib path} -lpir-go-arm64;
   #若直接执行可以使用
   DYLD_LIBRARY_PATH="${your lib path}";CGO_LDFLAGS="-L${your lib path} -lpir-go-arm64" go run main.go
   ```

