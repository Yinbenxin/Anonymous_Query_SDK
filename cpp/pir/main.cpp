#include <iostream>
#include <random>
#include <chrono>
#include <sstream>
#include "pir_client.h"
#include "pir_server.h"


using namespace std;
using namespace std::chrono;

void creat_data_to_file(int number_items, int size_per_item, string data_path){
    random_device rd;
    vector<vector<uint8_t>> data;
    for (int i = 0; i < number_items; i++) {
    std::vector<uint8_t> temp_data(size_per_item);

    for (int j = 0; j < size_per_item; j++) {
        temp_data[j] = rd() % 200 + 33;
    }
    data.push_back(temp_data);
    }
    vector<string> data_name(data.size());
    for (int i = 0; i < data.size(); ++i) {
        data_name[i] = data_path + to_string(i);
        if (!writeVectorToFile(data_name[i], data[i])) {
            cout << "write data file error" << endl;
            return;
        }
    }
}

int SEALPir_test_from_file( int number_items, int size_per_item) {
    std::vector<uint8_t> checked_bit;
    std::vector<uint8_t> query;
    std::vector<std::vector<uint8_t>> reply;
    std::vector<uint8_t> plaintext;
    std::vector<uint8_t> client_key;
    std::vector<uint8_t> server_key;

    random_device rd;
    unsigned int index = rd() % number_items;



    cout << "开始匿踪查询"<< endl;
    int err = 0;
    PirClient client(number_items, client_key, false, err);

    if (err != 0) {
        cout << "client 声明失败，错误码：" << err << endl;
        return err;
    }
    server_key = client.get_public_key();

    PirServer server(number_items, server_key, false, err);
    auto time_start = high_resolution_clock::now();

    err = client.generate_data_query(index, query);
    if (err != 0) {
        cout << "client产生query错误，错误码:" << err << endl;
        return err;
    }
    std::string data_path = "/Users/admin/git/Pir-SDK/cpp/cmake-build-debug/bin/data/";
    std::string reply_path = "/Users/admin/git/Pir-SDK/cpp/cmake-build-debug/bin/reply/reply";
    std::string result = "/Users/admin/git/Pir-SDK/cpp/cmake-build-debug/bin/result";

    creat_data_to_file(number_items, size_per_item, data_path);

    cout << "generate_reply_file" << endl;
    err = server.generate_reply_file(data_path, query, reply_path, 4);
    cout << "generate_data_reply" << endl;
    if (err != 0) {
        cout << "server产生reply失败，错误码:" << err << endl;
        return err;
    }
    reply.resize(1);

    err = client.decrypt_reply_file(reply_path, result, 4);
    if (err != 0) {
        cout << "client解密失败，错误码:" << err << endl;
        return err;
    }

    auto time_end = high_resolution_clock::now();
    readFileToVector(result, plaintext);

    vector<uint8_t> data_file;
    readFileToVector(data_path + to_string(index), data_file);
    for (int i = 0; i < data_file.size(); i++) {
        if (plaintext[i] != data_file[i]) {
            cout << "Main: PIR result wrong at " << i << endl;
        }
    }
    auto time_total = duration_cast<microseconds>(time_end - time_start).count();
    cout << "测试成功！" << endl;
    cout << "计算耗时: " << time_total / 1000 << " ms" << endl;
    auto reply_size =0;
    auto query_size =0;
    for (int i = 0; i < reply.size(); ++i) {
        reply_size += reply[i].size();
    }

    query_size = query.size();

    cout << "通信开销:" << reply_size + query_size << " Byte, 总明文大小" << number_items * plaintext.size() << " Byte" << endl;

    return 0;
}

int SEALPir_test(int number_items, int size_per_item) {
    std::vector<std::vector<uint8_t>> data;
    std::vector<uint8_t> checked_bit;
    std::vector<uint8_t> query;
    std::vector<std::vector<uint8_t>> reply;
    std::vector<uint8_t> plaintext;
    std::vector<uint8_t> client_key;
    std::vector<uint8_t> server_key;

    random_device rd;
    unsigned int index = rd() % number_items;
    data.reserve(number_items);

    for (int i = 0; i < number_items; i++) {
        std::vector<uint8_t> temp_data(size_per_item);
        std::vector<uint8_t> temp_id(20);

        for (int j = 0; j < size_per_item; j++) {
            temp_data[j] = rd() % 255 + 1;
        }
        data.push_back(temp_data);

        for (int j = 0; j < 20; j++) {
            temp_id[j] = rd() % 256;
        }
    }


    cout << "开始匿踪查询"<< endl;
    int err = 0;
    PirClient client(number_items, client_key, false, err);

    if (err != 0) {
        cout << "client 声明失败，错误码：" << err << endl;
        return err;
    }
    server_key = client.get_public_key();

    PirServer server(number_items, server_key, false, err);
    auto time_start = high_resolution_clock::now();

    err = client.generate_data_query(index, query);
    if (err != 0) {
        cout << "client产生query错误，错误码:" << err << endl;
        return err;
    }
    err = server.generate_data_reply(data, query, reply);
    if (err != 0) {
        cout << "server产生reply失败，错误码:" << err << endl;
        return err;
    }
//    reply.clear();
//    reply[0].resize(49996, 32);
    err = client.decrypt_data_reply(reply, plaintext);

    if (err != 0) {
        cout << "client解密失败，错误码:" << err << endl;
        return err;
    }
    auto time_end = high_resolution_clock::now();

    if (plaintext.size() != data[index].size()) {
        cout << "Main: PIR result wrong" << endl;
        return 0;
    }
    for (int i = 0; i < data[index].size(); i++) {
        if (plaintext[i] != data[index][i]) {
            cout << "Main: PIR result wrong at " << i << endl;
        }
    }
    auto time_total = duration_cast<microseconds>(time_end - time_start).count();
    cout << "测试成功！" << endl;
    cout << "计算耗时: " << time_total / 1000 << " ms" << endl;
    auto reply_size =0;
    auto query_size =0;
    for (int i = 0; i < reply.size(); ++i) {
        reply_size += reply[i].size();
    }

    query_size = query.size();

    cout << "通信开销:" << reply_size + query_size << " Byte, 总明文大小" << number_items * plaintext.size() << " Byte" << endl;

    return 0;
}


int main() {
    int err = SEALPir_test_from_file(5, 1000);
    if (err !=0){
        cout <<"client 测试失败，错误码："<<err<<endl;
    }
    return 0;
}
