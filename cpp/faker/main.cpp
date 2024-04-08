//
// Created by admin on 2024/1/8.
//
#include "iostream"
#include <vector>

#include "Faker.h"

int main() {
    int size = 10000;
    Faker faker;
    std::vector<std::vector<uint8_t>> IDs = faker.GenerateTWCompatriotNumber(size);
    int count = 0;  // 计数器
    for (auto &ID: IDs) {
        for (auto &byte: ID) {
            std::cout << byte;
        }
        std::cout << std::endl;
        count ++;
    }
    std::cout << "count: " << count << std::endl;
    return 0;
}