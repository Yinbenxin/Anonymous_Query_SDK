#include <pybind11/pybind11.h>
#include <pybind11/stl.h> // 包含对 std::vector 的支持
#include <vector>
#include <string>
#include <random>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include "pir_client.h"
#include "pir_server.h"
namespace py = pybind11;
using namespace pybind11::literals;
// Base64 编码函数

int secret_params= 4096;


std::string gen_sk(){
    int err = 0;
    std::vector<uint8_t> client_key;
    PirClient client(secret_params, client_key, false, err);
    std::vector<uint8_t> sk = client.get_private_key();
    std::string sk_str(sk.begin(), sk.end());
    return sk_str;
}
std::string gen_pk(){
    int err = 0;
    std::vector<uint8_t> client_key;
    PirClient client(secret_params, client_key, false, err);
    std::vector<uint8_t> pk = client.get_public_key();
    std::string pk_str(pk.begin(), pk.end());
    return pk_str;
}


std::string query(int num, std::string sk) {
    // 创建一个vector，长度等于源字符串的长度，并且每个元素都初始化为0
    std::vector<uint8_t> target_vec(sk.size());
    int err = 0;
    // 使用std::copy算法将源字符串复制到目标vector中
    std::copy(sk.begin(), sk.end(), target_vec.begin());
    PirClient client(secret_params, target_vec, false, err);
    std::vector<uint8_t> query;
    err = client.generate_data_query(num, query);
    std::string result(query.begin(), query.end());
    return result;
}

std::string reply(std::string query,  std::string pk, const std::vector<std::string>& str_list) {
    std::vector<uint8_t> target_vec(pk.size());
    int err = 0;
    // 使用std::copy算法将源字符串复制到目标vector中
    std::copy(pk.begin(), pk.end(), target_vec.begin());
    PirServer server(secret_params, target_vec, false, err);
    std::vector<std::vector<uint8_t>> data(str_list.size());
    for (int i = 0; i < str_list.size(); ++i) {
        std::vector<uint8_t> ele(str_list[i].size());
        std::copy(str_list[i].begin(), str_list[i].end(), ele.begin());
        data[i] = ele;
    }
    std::vector<uint8_t> query_vec(query.size());
    // 使用std::copy算法将源字符串复制到目标vector中
    std::copy(query.begin(), query.end(), query_vec.begin());
    PirClient client(secret_params, target_vec, false, err);
    std::vector<std::vector<uint8_t>> reply;
    err = server.generate_data_reply(data, query_vec, reply);

    std::string result(reply[0].begin(), reply[0].end());
    return result;
}

std::string decrypt(std::string reply, std::string sk) {
    std::vector<uint8_t> target_vec(sk.size());
    int err = 0;
    // 使用std::copy算法将源字符串复制到目标vector中
    std::copy(sk.begin(), sk.end(), target_vec.begin());
    PirClient client(secret_params, target_vec, false, err);
    std::vector<uint8_t> reply_vec(reply.size());
    // 使用std::copy算法将源字符串复制到目标vector中
    std::copy(reply.begin(), reply.end(), reply_vec.begin());
    std::vector<uint8_t> plaintext;
    std::vector<std::vector<uint8_t>> reply_vecs;
    reply_vecs[0] = reply_vec;
    err = client.decrypt_data_reply(reply_vecs, plaintext);
    std::string result(plaintext.begin(), plaintext.end());
    return result;
}

PYBIND11_MODULE(add, m) {
    m.def("gen_pk", &gen_pk);
    m.def("gen_sk", &gen_sk);
    m.def("query", &query);
    m.def("reply", &reply);
    m.def("decrypt", &decrypt);
}

