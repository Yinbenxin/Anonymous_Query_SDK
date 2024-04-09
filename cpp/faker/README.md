# 膨胀数据的C++实现：Faker

Faker 是一个用于生成假数据的C++包，针对公司业务，目前支持中国居民身份证、台胞证、台湾身份证、台湾大陆居民身份证、中国手机号、mac地址、imei、imsi、email、护照的仿真数据生成。

### 编译方法

项目目录下：

```
mkdir build & cd build
cd build
cmake ..
make
```

### 使用方法：

将编译好的libfaker.so文件和include文件夹中的头文件导入你的项目后，在你的项目中引入头文件`FakerFactory.h` 

```
Faker* Faker = CreateFaker();
//生成一个大陆身份证号
std::string idCardNo = Faker->GenerateCNIDCardNumber();
```

在tests文件夹中有一个例子，演示了所有方法。

### 支持的FAKES数据：

```
std::string GenerateCNIDCardNumber();    //18位中国居民身份证
std::string GenerateTWCompatriotNumber();  //8位台胞证
std::string GenerateTWIDCardNumber();      //10位台湾身份证
std::string GenerateTW2CNIDCardNumber();    //18位台湾大陆居民身份证
std::string GeneratePhoneNumber();          //11位中国电话号码
std::string GenerateMACAddress();          //Mac地址，随机6字节，如08：00：20：0A：8C：6D
std::string GenerateIMEI();               //15位 IMEI
std::string GenerateIMSI();               //15位 IMSI
std::string GenerateEmail();                //不定长邮箱，字母+数字+后缀
std::string GeneratePassportNumber();        //护照，日本，新加坡，马来西亚，法国，美国和中国护照按比例随机产生
```

### 生成规则：

- 中国居民身份证，举例：11010119900101001X

  - 第1位至第6位：行政区划代码，代表省、自治区或直辖市。

  - 第7位至第14位：出生年月日。

  - 第15位至第17位：顺序码，用于区分同一天出生的不同人。

  - 第18位：校验码，用于验证身份证号码的合法性。

- 台胞证，举例：49020866
  - 随机的8位数字。

- 台湾身份证，举例：Y262829389

  - 第1位：地区A-Z

  - 第2位：性别位，1为男，2为女

  - 第3位至第9位：随机的7位数字

  - 第10位：校验码

- 台湾大陆身份证号码，举例：830000199812164330

  - 第1位至第6位：行政区划代码，代表省、自治区或直辖市，台湾的为830000。

  - 第7位至第14位：出生年月日。

  - 第15位至第17位：顺序码，用于区分同一天出生的不同人。

  - 第18位：校验码，用于验证身份证号码的合法性。

- 电话号码，举例：13820182930

  - 第1位至第3位：电话号码前缀

  - 第4至11位：随机值

- MAC地址，举例：08：00：20：0A：8C：6D
  - 长度为48位(6个字节)，通常表示为12个16进制数，中间以“:”分隔开
- IMEI，举例：626104121747928
  - 第1位至第14位：随机数字
  - 第15位：校验位
- IMSI，举例：460022292584063
  - 第1位至第3位：移动国家代码mcc，中国为460
  - 第4位至第5位：移动网络代码cnm，从中国的移动网络代码可选项中随机抽取
  - 第6位至第15位：随机数
- 邮箱，举例：geaab00@outlook.com
  - 随机5至10位的字母
  - 随机0至5个数字
  - 中国常用邮箱后缀
- 护照，举例：EM9854722，S3822651Q
  - 目前支持日本，新加坡，马来西亚，法国，美国和中国的生成规则，按比例随机产生，2/3的概率是中国护照。
  - 中国规则：“E"+一个除I和O的大写字母+7位随机数
  - 日本护照：在七位号码前有两个英文字母。
  - 新加坡护照：护照号码前是大写“S”中间七位数字，最后有一位英文字母。
  - 马来西亚护照：英文字母“K”字打头， 加七位数字。
  - 法国护照：前两位是数字，中间是两个英文字母，最后为五位数字。
  - 美国护照：九位阿拉伯数字。