//
// Created by admin on 2023/11/9.
//

#include "pir_client.h"

using namespace std;
using namespace seal;


PirClient::PirClient(int anonymity_number, const std::vector<uint8_t> &input_key,
                     bool is_checked_count, int& error_code) {
    if (anonymity_number <= 1) {
        error_code = E_NUMBER_OR_DATA_SIZE_INVALID;
        return;
    }
    number_of_items = anonymity_number;
    data_size = Seal_insert_max_size;
    pir_client = std::make_unique<PIRClient>(number_of_items, data_size);
    error_code = 0;

    if (input_key.empty()) {
        generate_key();
    } else {
        private_key = input_key;
    }
    try {
        pir_client->loadKey(private_key);
    }
    catch (...) {
        error_code = E_PRIVATE_KEY_ERROR;
        return;
    }

    error_code = 0;
}


int PirClient::set_key(const std::vector<uint8_t> &input_key) {
    private_key = input_key;
    try {
        pir_client->loadKey(private_key);
    }
    catch (...) {
        return E_PRIVATE_KEY_ERROR;
    }

    return 0;
}

std::vector<uint8_t> PirClient::get_private_key() {
    return private_key;
}

std::vector<uint8_t> PirClient::get_public_key() {
    return public_key;
}

int PirClient::generate_data_query(int index, std::vector<uint8_t> &data_query) {
    if (index < 0 || index >= number_of_items) {
        return E_INIT_PARAMETER_ERROR;
    }
    data_query.resize(1);
    try {
        pir_client->GenerateQuery(data_query, index);
    }
    catch (...) {
        return E_QUERY_ERROR;
    }

    return 0;
}


int PirClient::decrypt_data_reply(const std::vector<std::vector<uint8_t>> &data_reply, std::vector<uint8_t> &result) {

    for (int i = 0; i < data_reply.size(); ++i) {
        if (!data_reply[i].empty()) {
            try {
                PirReply reply = pir_client->deserialize_reply(data_reply[i]);
                auto data = pir_client->decode_reply(reply, 0);
                result.insert(result.end(), data.begin(), data.end());
            }
            catch (...) {
                return E_REPLY_ERROR;
            }
        }
    }

    // 删除尾部的空格元素，为什么？
    int idx = result.size() - 1;
    while (idx >= 0 && result[idx] == 32) {
        idx--;
    }
    if (idx + 1 >= 0) {
        result.resize(idx + 1);
    }
    return 0;
}


int PirClient::generate_key() {
    pir_client->GenerateKey(private_key, public_key);
    return 0;
}



int PirClient::decrypt_reply_(int index_thread, int task_number_per_thread, int mod_thread, char *inputFile,
                  std::ofstream &outfile, int &err) {
    // 确定文件读取的起始位置z
    std::vector<std::vector<uint8_t>> index;
    char lenBuff[4];
    char reply_buff[reply_max_size]; //读取数据的缓冲区，每次读取10000字节
    int read_start_index = index_thread * task_number_per_thread * reply_max_size;

    std::ifstream read_file(inputFile, std::ios::in);
    if (!read_file.is_open()) {
        // 文件打开失败，记录日志并退出程序
        err = E_OPEN_FILE_FAILED;
    }

    std::vector<std::vector<uint8_t>> reply(1);
    if (index_thread == task_number_per_thread) {
        task_number_per_thread += mod_thread;
    }

    uint32_t len = 0;
    std::vector<std::vector<uint8_t>> data_result(number_of_items);
    std::vector<uint8_t> plaintext;
    for (int i = 0; i < task_number_per_thread; ++i) {
        if (!read_file.seekg(read_start_index, std::ios::beg)) {
            // seekp 失败，记录日志并退出程序
            err = E_SEEK_FILE_FAILED;
            return err;
        }
        read_file.read(lenBuff, 4);
        if (read_file.gcount() <= 0) {
            break;
        }
        len = *((uint32_t *) lenBuff);
        if (len > reply_max_size) {
            err = E_INPUT_FILE_ERROR;
            return err;
        }
        read_file.read(reply_buff, len);
        if (read_file.gcount() != len) {
            err = E_INPUT_FILE_ERROR;
            return err;
        }
        reply[0].clear();
        reply[0].insert(reply[0].end(), reply_buff, reply_buff + len);
        plaintext.clear();
        err = decrypt_data_reply(reply, plaintext);
        if (err != 0) {
            return err;
        }
        outfile.write((char *) plaintext.data(), plaintext.size());
        read_start_index += reply_max_size;
    }
    read_file.close();
    return 0;
}

int PirClient::decrypt_reply_file(string reply_file, string output, int number_threads) {
    if (reply_file.size() == 0 || output.size() == 0) {
        return E_INPUT_DATA_ERROR;
    }
    if (number_threads <= 0) {
        return E_NUMBER_THREADS_ERROR;
    }
    if (number_threads < 1) {
        return E_NUMBER_THREADS_ERROR;
    }
    int err = 0;

    std::vector<std::vector<uint8_t>> replyData;
    std::vector<uint8_t> preReply;
    replyData.push_back(preReply);
    std::vector<uint8_t> plainData;
    struct stat st;
    stat(reply_file.c_str(), &st);
    int data_max_size = st.st_size;


    int number_buff = std::ceil(static_cast<double>(data_max_size) / reply_max_size); // 一共有多少个块
    int number_buff_per_thread = number_buff / number_threads; // 每个线程处理多少个块, 多余的数据都给最后一个线程处理，微乎其微。
    int mod_thread = number_buff % number_threads; // 余数

    std::vector<std::ofstream> outfiles;

    // 外部需要将待写入的文件创建好，这里只负责打开并定位，若多个线程边跳转边写入会出现碰撞。
    for (int i = 0; i < number_threads; ++i) {
        std::ofstream file(output.c_str(), std::ios::out);
        if (!file.is_open()) {
            return E_OPEN_PLAINT_OUT_FILE_FAILED;
        }
        if (!file.seekp(i * number_buff_per_thread * buff_size, std::ios::beg)) {
            // seekp 失败，记录日志并退出程序
            std::cerr << "Failed to seek file: " << std::endl;
            return E_SEEK_FILE_FAILED;
        }
        outfiles.push_back(std::move(file));
    }


    std::vector<std::thread> threads;
    std::vector<int> errs(number_threads, 0);

    for (int i = 0; i < number_threads; ++i) {
        threads.emplace_back(
                std::bind(&PirClient::decrypt_reply_, this, i, number_buff_per_thread, mod_thread, (char *)reply_file.c_str(),
                          std::ref(outfiles[i]), std::ref(errs[i])));
    }

    for (auto &thread: threads) {
        thread.join();
    }
    for (auto er: errs) {
        if (er != 0) {
            return er;
        }
    }
    for (auto &file: outfiles) {
        file.close();
    }


    return err;
}





