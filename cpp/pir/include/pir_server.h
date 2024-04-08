//
// Created by admin on 2023/11/9.
//

#pragma once

#include "seal_server.h"
#include "errorcode.h"


/**
 * SEALPirServer是SEAL算法的批量匿踪查询实现类。
 */
class PirServer {
public:
    /**
     * SEALPirServer构造函数。
     * @param anonymity_number 数据集合的行数。
     * @param key 用于加密和解密的密钥。
     * @param is_checked_count 是否统计查得数量。
     * @param error_code 错误码。
     */
    PirServer(int anonymity_number, const std::vector<uint8_t> key, bool is_checked_count, int &error_code);

    /**
     * SEALPirServer析构函数。
     */
    ~PirServer();

    /**
     * generate_data_reply产生想要查询数据的密文。
     * @param data 数据方返回的查询数据。
     * @param data_query 密文数据请求————from client。
     * @param data_reply 密文响应结果————to client。
     * @return 如果查得返回查询数量，如果非查得返回1。
     */
    int generate_data_reply(const std::vector<std::vector<uint8_t>> &data,
                            const std::vector<uint8_t> &data_query,
                            std::vector<std::vector<uint8_t>> &data_reply);
    /**
     * @param data_file     数据文件
     * @param query_file    请问文件
     * @param reply_file    返回文件
     * @param core          核心数
     * @return
     */
    int generate_reply_file(const std::string data_file, const std::vector<uint8_t> &data_query,
                            const std::string reply_file, int core=1);

    /**
     * set_key设置密钥。
     * @param private_key 用于加密和解密的密钥。
     */
    int set_key(const std::vector<uint8_t> &private_key);

    /**
     * get_private_key获取私钥。
     * @return 返回私钥。
     */
    std::vector<uint8_t> get_private_key();

    /**
     * get_public_key获取公钥。
     * @return 返回公钥。
     */
    std::vector<uint8_t> get_public_key();

private:
    int generate_reply_(int index_thread, int number_buff_per_thread, int mod_thread,
                        std::string inputFile, std::string outputFile,
                        const std::vector<uint8_t> &data_query, int &err);

    std::vector<uint8_t> private_key;// 私钥
    std::vector<uint8_t> public_key;// 公钥
    int number_of_items;// 数据集合的行数
    int data_size;//
    std::unique_ptr<PIRServer> pir_server;
};

