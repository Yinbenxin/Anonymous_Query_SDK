//
// Created by admin on 2023/11/9.
//
#pragma once

#include "seal_client.h"
#include "errorcode.h"

/**
 * PIR（Private Information Retrieval）Server是批量匿踪查询抽象类，实现了n选1的查询。
 * 基于seal实现的匿踪查询算法。
 * 查询方想从n行数据中选取其中1行，将想要查询的数据放在其中一行。
 */
class PirClient {
public:
    /**
     * SEALPirClient构造函数。
     * @param anonymity_number 数据行数。
     * @param key 用于加密和解密的密钥。
     * @param is_checked_count 是否检查查询结果的个数。
     * @param error_code 错误码。
     */
    PirClient(int anonymity_number, const std::vector<uint8_t> &key, bool is_checked_count, int& error_code);


    /**
     * generate_data_query将想要查询的索引转换为密文请求。
     * @param indexes 混淆后索引集合，一共anonymity_number行，其中一行是需要查询的数据。
     * @param position 查询数据所在位置。
     * @param data_query 查询索引转换的密文请求
     * @return 错误码
     */
    int generate_data_query(int index, std::vector<uint8_t> &data_query);

    /**
     * decrypt_data_reply解密数据密文获得查询结果。
     * @param data_reply
     * @param result
     * @return 错误码
     */
    int decrypt_data_reply(const std::vector<std::vector<uint8_t>> &data_reply,
                           std::vector<uint8_t> &result);


    /**
     * set_key设置密钥。
     * @param private_key
     * @return 错误码
     */
    int set_key(const std::vector<uint8_t> &private_key);

    /**
     * get_private_key获取私钥。
     * @return
     */
    std::vector<uint8_t> get_private_key();

    /**
     * get_public_key获取公钥。
     * @return
     */
    std::vector<uint8_t> get_public_key();
    int decrypt_reply_file(std::string reply_file, std::string output, int core = 1);

private:
    /**
     * generate_key生成密钥。
     * @return 错误码
     */
    int generate_key();
    int decrypt_reply_(int index_thread, int task_number_per_thread, int mod_thread, char *inputFile,
                                  std::ofstream &outfile, int &err);

    std::vector<uint8_t> private_key;// 私钥
    std::vector<uint8_t> public_key;// 公钥
    int number_of_items;// 数据行数
    int data_size;// 数据行数
    std::unique_ptr<PIRClient> pir_client;
};



