//
// Created by admin on 2023/10/26.
//
#include "Util.h"
#include <ctime>
#include <random>
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
static std::random_device rd;
static std::default_random_engine gen(rd());

int RandomNum(int min, int max) {
    std::uniform_int_distribution<int> dis(min, max);
    int random = dis(gen);
    return random;
}

std::string GetRandomIntStr(int lenth){
    std::string res;
    for(int i = 0;i<lenth;i++){
        res+=std::to_string(RandomNum(0, 9));
    }
    return res;
}

std::string GetTBRandomIntStr(){
    std::random_device rd; // 获取随机数种子
    std::mt19937 gen(rd()); // 用于生成随机数
    std::uniform_int_distribution<> dist(1, 1209); // 总共有1175+3+32=1210个数，为了方便计算，这里使用1~1209

    int number = dist(gen);
    int FirstFourDigits= 0;
    if (number <= 1175) { // 对应0000～1174
        FirstFourDigits = number - 1; // 因为我们的范围是从1开始的，所以这里减1
    } else if (number <= 1178) { // 对应2000～2002
        FirstFourDigits = 2000 + (number - 1176);
    } else { // 对应3000～3031
        FirstFourDigits = 3000 + (number - 1179);
    }
    std::ostringstream oss;
    oss << std::setw(4) << std::setfill('0') << FirstFourDigits;
    std::string result = oss.str() + GetRandomIntStr(4);        //台胞证位8位随机值
    return result;
}

int generateRandomYear() {
    // 将每个地区码出现次数转化为概率分布
    std::random_device rd;
    std::mt19937 gen(rd());
    std::discrete_distribution<> d(yearRate.begin(), yearRate.end());
    return 1962 + d(gen);
}


std::string RandomDate(std::tm start, std::tm end) {
    std::time_t start_time = std::mktime(&start);
    std::time_t end_time = std::mktime(&end);
    std::uniform_int_distribution<std::time_t> dis(start_time, end_time);
    std::time_t random_time = dis(gen);
    std::tm* random_tm = std::localtime(&random_time);
    char buffer[11];
    std::strftime(buffer, sizeof(buffer), "%Y%m%d", random_tm);
    return std::string(buffer);
}
std::string generateRandomDate(int year) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::discrete_distribution<> d(monthRate.begin(), monthRate.end());
    // 生成随机的月份（1月至12月之间）
    int month = d(gen) + 1;

    // 根据月份生成对应的天数
    int day;
    if (month == 2) {
        // 对于2月份，根据年份判断是否为闰年
        bool isLeapYear = ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0);
        day = isLeapYear ? std::rand() % 29 + 1 : std::rand() % 28 + 1;
    } else if (month == 4 || month == 6 || month == 9 || month == 11) {
        day = std::rand() % 30 + 1;
    } else {
        day = std::rand() % 31 + 1;
    }

    // 将日期转换为字符串
    std::string monthStr = (month < 10) ? "0" + std::to_string(month) : std::to_string(month);
    std::string dayStr = (day < 10) ? "0" + std::to_string(day) : std::to_string(day);

    std::string date = std::to_string(year) + monthStr + dayStr;
    return date;
}



std::string generateRegionCodeWithWeight() {
    // 将每个顺序码出现次数转化为概率分布
    std::random_device rd;
    std::mt19937 gen(rd());
    std::discrete_distribution<> d(regionRate.begin(), regionRate.end());
    return regionData[d(gen)];
}
std::string RandomRegistryCode() {
    auto num = RandomNum(0, 0);
    if (num < 10) {
        return std::to_string(0) + std::to_string(num);
    }
    return std::to_string(num);
}
std::string RandomRegistry() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::discrete_distribution<> d(RegistryRate50.begin(), RegistryRate50.end());
    int num = 1 + d(gen);
    std::stringstream ss;
    ss << std::setw(3) << std::setfill('0') << num;
    return ss.str();
}
char CheckCodeCN(std::string id) {
    if (id.size() != 17) {
        return -1;
    }
    for (int i = 0; i < 16; ++i) {
        if (id[i] < '0' || id[i] > '9') {
            return -1;
        }
    }
    int sum = 0;
    for (int i = 0; i < 17; ++i) {
        sum += (id[i] - '0') * kFactors[i];
    }
    return kCheckDigits[sum % 11];
}

char CheckCodeTW(std::string id) {
    if (id.size() != 9) {
        return -1;
    }
    for (int i = 1; i < 9; ++i) {
        if (id[i] < '0' || id[i] > '9') {
            return -1;
        }
    }
    int sum = 0;
    sum += TWLocalCode[(id[0] - 'A')]/10 * TWkFactors[0];
    sum += TWLocalCode[(id[0] - 'A')]%10 * TWkFactors[1];
    for (int i = 1; i < 9; ++i) {
        sum += (id[i] - '0') * TWkFactors[i+1];
    }
    return (10-sum%10)%10+'0';
}