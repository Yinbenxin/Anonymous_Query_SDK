一、说明

本库基于现有算法（如OPRF、SEAL）提供匿踪能力接口，对匿踪算法进行抽象。所有匿踪算法均为原子化接口即实现n选1的查询方式，当然也可以实现批量，即n行选1，将想要查询的数据放在一行中。另外基于现实需求，本库还支持批量查询方式，即数据方获得查询方查询数量，但无法知晓查询内容。

二、算法抽象

![flow-abstraction](../docs/_static/flow-abstraction.png)

![class-abstraction](../docs/_static/class-abstraction.png)

四、依赖库

1. seal 4.0.0

   ```bash
   wget https://github.com/microsoft/SEAL/archive/refs/tags/v4.0.0.zip
   unzip v4.0.0.zip
   cd SEAL-4.0.0
   cmake -S . -B build
   cmake --build build
   sudo cmake --install build
   ```

2. swig

   ```bash
   wget https://github.com/swig/swig/archive/refs/tags/v4.1.1.zip
   cmake -S . -B build
   cmake --build build
   sudo cmake --install build
   ```

五、性能

测试机器：macM1Pro 16GB。

1、单核下计算性能对比

![单核下计算性能对比](../docs/_static/%E5%8D%95%E6%A0%B8%E4%B8%8B%E8%AE%A1%E7%AE%97%E6%80%A7%E8%83%BD%E5%AF%B9%E6%AF%94.png)

2.不同并发数下计算时间关系

![不同并发数下计算时间关系](../docs/_static/%E4%B8%8D%E5%90%8C%E5%B9%B6%E5%8F%91%E6%95%B0%E4%B8%8B%E8%AE%A1%E7%AE%97%E6%97%B6%E9%97%B4%E5%85%B3%E7%B3%BB.png)

3.通信开销

![通信开销](../docs/_static/%E9%80%9A%E4%BF%A1%E5%BC%80%E9%94%80.png)

4.子函数性能

![sm4-AES性能](../docs/_static/sm4-AES%E6%80%A7%E8%83%BD.png)

![oprftime](../docs/_static/oprftime.png)
