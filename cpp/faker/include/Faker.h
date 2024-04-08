#include <vector>

#ifndef FAKER_
#define FAKER_

class Faker {
public:
    Faker();

    std::vector<std::vector<uint8_t>> GenerateCNIDCardNumber(int number=1);

    std::vector<std::vector<uint8_t>> GenerateTWCompatriotNumber(int number=1);

    std::vector<std::vector<uint8_t>> GenerateTWIDCardNumber(int number=1);

    std::vector<std::vector<uint8_t>> GenerateTW2CNIDCardNumber(int number=1);

    std::vector<std::vector<uint8_t>> GeneratePhoneNumber(int number=1);

    std::vector<std::vector<uint8_t>> GenerateMACAddress(int number=1);

    std::vector<std::vector<uint8_t>> GenerateIMEI(int number=1);

    std::vector<std::vector<uint8_t>> GenerateIMSI(int number=1);

    std::vector<std::vector<uint8_t>> GenerateEmail(int number=1);

    std::vector<std::vector<uint8_t>> GeneratePassportNumber(int number=1);


};

#endif

