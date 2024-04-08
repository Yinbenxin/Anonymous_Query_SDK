#include "Util.h"
#include "Faker.h"
#include <iomanip>
#include <vector>
#include <set>
#include <sstream>
#include <random>


const std::tm start = { 0, 0, 0, 1, 0, 61, 0, 0, 0 }; // 1960年1月1日
const std::tm end = { 0, 0, 0, 1, 0, 105, 0, 0, 0 }; // 2008年1月1日
const std::vector<std::string> CNM_NC_Code= {"00","02","04","07","08","01","06","09","03","05","11"}; //中国 MNC 代码
const std::vector<std::string> phonePre = {"130", "131", "132", "133", "134", "135", "136", "137", "138", "139", "147", "150", "151", "152", "157", "158", "159", "172", "178", "182", "183", "184", "187", "188", "198"};
std::random_device rd;
std::mt19937 gen(rd());
std::vector<std::vector<uint8_t>> set2vector(std::set<std::string> set){
    std::vector<std::vector<uint8_t>> results;
    for (const auto& str : set) {
        std::vector<uint8_t> result(str.c_str(), str.c_str() + str.size()); // 将字符串转换为字节向量
        results.push_back(result);
    }
    return results;
}
Faker::Faker() { return;}
//随机生成的方式：46.2%
std::vector<std::vector<uint8_t>> Faker::GenerateCNIDCardNumber(int number){     //中国身份证号码生成
    std::set<std::string> uniqueResults; // 用于存储非重复的结果
    int count = 0;
    while (uniqueResults.size() < number) {
        std::string result;
        // 生成身份证号码字符串
        result += generateRegionCodeWithWeight();
        result += generateRandomDate(generateRandomYear());
        result += RandomRegistry();
        result += CheckCodeCN(result);
        uniqueResults.insert(result); // 将结果插入set中，set会自动去重
        if (count++ > number* 10000) {
            break;
        }
    }
    return set2vector(uniqueResults);
}

std::vector<std::vector<uint8_t>> Faker::GenerateTWCompatriotNumber(int number){
    std::set<std::string> uniqueResults; // 用于存储非重复的结果
    int count = 0;
    while (uniqueResults.size() < number) {
        uniqueResults.insert(GetTBRandomIntStr()); // 将结果插入set中，set会自动去重
        if (count++ > number* 10000) {
            break;
        }
    }
    return set2vector(uniqueResults);
}
std::vector<std::vector<uint8_t>> Faker::GenerateTWIDCardNumber(int number){      //台湾身份证
    std::set<std::string> uniqueResults; // 用于存储非重复的结果
    int count = 0;
    while (uniqueResults.size() < number) {
        std::string result;

        result += char(RandomNum('A', 'Z'));              //台湾身份证首先先选地区
        result += std::to_string(RandomNum(0, 9) % 2 + 1);   //1为男，2为女
        result += GetRandomIntStr(7); //随机7位
        result += CheckCodeTW(result); //校验码
        uniqueResults.insert(result); // 将结果插入set中，set会自动去重
        if (count++ > number* 10000) {
            break;
        }
    }
    return set2vector(uniqueResults);
}
std::vector<std::vector<uint8_t>> Faker::GenerateTW2CNIDCardNumber(int number){     //台湾大陆身份证号码生成
    std::set<std::string> uniqueResults; // 用于存储非重复的结果
    int count = 0;
    while (uniqueResults.size() < number) {
        std::string result;
        result += "830000";       //台湾6位地区位
        result += RandomDate(start, end);//随机出生日期 8位
        result += RandomRegistryCode();  //两位顺序码，暂时没找到可用规律
        result += std::to_string(RandomNum(1, 6)); //性别码1位，1-6概率更高
        result += CheckCodeCN(result); //1位校验位
        uniqueResults.insert(result); // 将结果插入set中，set会自动去重
        if (count++ > number* 10000) {
            break;
        }
    }
    return set2vector(uniqueResults);
}
std::vector<std::vector<uint8_t>> Faker::GeneratePhoneNumber(int number){     //电话号码，前缀加随机8位
    std::set<std::string> uniqueResults; // 用于存储非重复的结果
    int count = 0;
    while (uniqueResults.size() < number) {
        std::string result;
        result += phonePre[RandomNum(0, phonePre.size() - 1)];
        result += GetRandomIntStr(8);
        uniqueResults.insert(result); // 将结果插入set中，set会自动去重
        if (count++ > number* 10000) {
            break;
        }
    }

    return set2vector(uniqueResults);
}
std::vector<std::vector<uint8_t>> Faker::GenerateMACAddress(int number){        //Mac地址，随机6字节，如08：00：20：0A：8C：6D

    std::set<std::string> uniqueResults; // 用于存储非重复的结果
    int count = 0;
    while (uniqueResults.size() < number) {
        std::uniform_int_distribution<int> dis(0, 255);
        std::stringstream mac;
        mac << std::hex << std::setfill('0');
        for (int i = 0; i < 6; ++i) {
            mac << std::setw(2) << dis(gen);
            if (i != 5) {
                mac << ":";
            }
        }
        uniqueResults.insert(mac.str()); // 将结果插入set中，set会自动去重
        if (count++ > number* 10000) {
            break;
        }
    }


    return set2vector(uniqueResults);
}
std::vector<std::vector<uint8_t>> Faker::GenerateIMEI(int number){        //IMEI
    std::set<std::string> uniqueResults; // 用于存储非重复的结果
    int count = 0;

    while (uniqueResults.size() < number) {
        std::string imei;
        // 生成前14位的随机数字
        imei+= GetRandomIntStr(14);
        // 计算校验位
        int sum = 0;
        for (int i = 0; i < 14; i++) {
            int digit = imei[i] - '0';
            if (i % 2 == 0) {
                digit *= 2;
                if (digit >= 10) {
                    digit = digit / 10 + digit % 10;
                }
            }
            sum += digit;
        }
        int checkDigit = (10 - (sum % 10)) % 10;
        imei += std::to_string(checkDigit);
        uniqueResults.insert(imei); // 将结果插入set中，set会自动去重
        if (count++ > number* 10000) {
            break;
        }
    }

    return set2vector(uniqueResults);
}
std::vector<std::vector<uint8_t>> Faker::GenerateIMSI(int number){     //IMSI=mcc+cnm+随机10位数字

    std::set<std::string> uniqueResults; // 用于存储非重复的结果
    int count = 0;
    while (uniqueResults.size() < number) {
        std::string imsi;
        std::string mcc = "460"; // 中国的移动国家代码
        imsi+= mcc + CNM_NC_Code[RandomNum(0, CNM_NC_Code.size() - 1)] + GetRandomIntStr(10);
        uniqueResults.insert(imsi); // 将结果插入set中，set会自动去重
        if (count++ > number* 10000) {
            break;
        }
    }

    return set2vector(uniqueResults);
}
std::vector<std::vector<uint8_t>> Faker::GenerateEmail(int number){        //邮箱，字母+数字+后缀

    std::set<std::string> uniqueResults; // 用于存储非重复的结果

    int count = 0;
    while (uniqueResults.size() < number) {
        std::uniform_int_distribution<> dis(0, 9);
        // 常用邮箱后缀
        std::vector<std::string> emailSuffixes = {
                "@qq.com",
                "@163.com",
                "@126.com",
                "@sina.com",
                "@gmail.com",
                "@hotmail.com",
                "@outlook.com"
        };
        std::string username;
        int usernameLenth = RandomNum(5, 10);
        for (int i = 0; i < usernameLenth; ++i) {
            username += 'a' + dis(gen);
        }
        int end_number_lenth = RandomNum(0, 5);
        username+= GetRandomIntStr(end_number_lenth);
        // 随机选择邮箱后缀
        std::uniform_int_distribution<> suffixDis(0, emailSuffixes.size() - 1);
        std::string suffix = emailSuffixes[suffixDis(gen)];

        // 组合生成邮箱地址
        std::string email = username + suffix;

        uniqueResults.insert(email); // 将结果插入set中，set会自动去重
        if (count++ > number* 10000) {
            break;
        }
    }


    return set2vector(uniqueResults);
}
std::vector<std::vector<uint8_t>> Faker::GeneratePassportNumber(int number){     //护照，目前支持日本，新加坡，马来西亚，法国，美国和中国的生成规则，按比例随机产生。

    std::set<std::string> uniqueResults; // 用于存储非重复的结果
    int count = 0;
    while (uniqueResults.size() < number) {
        std::string passportNumber;
        int index = RandomNum(1, 15);  //1-5是国外的护照，6-15是中国护照，考虑到基数问题，2/3的概率是中国护照
        switch (index) {
            case 1:  // 日本护照
            {
                std::uniform_int_distribution<> letterDis(0, 25);
                char letter1 = 'A' + letterDis(gen);
                char letter2 = 'A' + letterDis(gen);
                passportNumber = std::string(1, letter1) + std::string(1, letter2);

                for (int i = 0; i < 7; ++i) {
                    passportNumber += std::to_string(RandomNum(0, 9));
                }
                break;
            }
            case 2:  // 新加坡护照
            {
                std::uniform_int_distribution<> letterDis(0, 25);
                char letter = 'A' + letterDis(gen);
                passportNumber = "S";

                for (int i = 0; i < 7; ++i) {
                    passportNumber += std::to_string(RandomNum(0, 9));
                }

                passportNumber += std::string(1, letter);
                break;
            }
            case 3:  // 马来西亚护照
            {
                passportNumber = "K";

                for (int i = 0; i < 7; ++i) {
                    passportNumber += std::to_string(RandomNum(0, 9));
                }
                break;
            }
            case 4:  // 法国护照
            {
                std::uniform_int_distribution<> letterDis(0, 25);
                std::uniform_int_distribution<> digitDisFrance(0, 99999);
                int digits = digitDisFrance(gen);
                char letter1 = 'A' + letterDis(gen);
                char letter2 = 'A' + letterDis(gen);
                passportNumber = std::to_string(digits / 10000) + std::to_string((digits / 1000) % 10) +
                                 std::string(1, letter1) + std::string(1, letter2) +
                                    std::to_string(digits % 1000);
                break;
            }
            case 5:  // 美国护照
            {
                for (int i = 0; i < 9; ++i) {
                    passportNumber += std::to_string(RandomNum(0, 9));
                }
                break;
            }
            default:      //中国护照
                std::uniform_int_distribution<> letterDis(0, 25);
                char letter;
                do {
                    letter = 'A' + letterDis(gen);
                } while (letter == 'I' || letter == 'O');

                passportNumber = "E" + std::string(1, letter);

                for (int i = 0; i < 7; ++i) {
                    passportNumber += std::to_string(RandomNum(0, 9));
                }
                break;
        }
        uniqueResults.insert(passportNumber); // 将结果插入set中，set会自动去重
        if (count++ > number* 10000) {
            break;
        }
    }

    return set2vector(uniqueResults);

}