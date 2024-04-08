//
// Created by admin on 2023/11/9.
//

#include "pir_server.h"


using namespace std;
using namespace seal;



PirServer::PirServer(int anonymity_number, const std::vector<uint8_t> input_key,
                     bool is_checked_count, int &error_code) {
    if (anonymity_number <= 0) {
        error_code = E_NUMBER_OR_DATA_SIZE_INVALID;
        return;
    }
//    int max_size_of_data = Seal_insert_max_size
    number_of_items = anonymity_number;
    data_size = Seal_insert_max_size;
    pir_server = std::make_unique<PIRServer>(number_of_items, data_size);
    public_key = input_key;
    try {
        pir_server->set_galois_key(public_key);
    }
    catch (...) {
        error_code = E_PUBLIC_KEY_ERROR;
        return;
    }
    error_code = 0;
}

PirServer::~PirServer() {
}

int PirServer::set_key(const std::vector<uint8_t> &input_key) {
    public_key = input_key;
    try {
        pir_server->set_galois_key(public_key);
    }
    catch (...) {
        return E_PUBLIC_KEY_ERROR;
    }
    return 0;
}

std::vector<uint8_t> PirServer::get_private_key() {
    return private_key;
}

std::vector<uint8_t> PirServer::get_public_key() {
    return public_key;
}

int PirServer::generate_data_reply(const std::vector<std::vector<uint8_t>> &data,
                                   const std::vector<uint8_t> &data_query,
                                   std::vector<std::vector<uint8_t>> &data_reply) {
    if (data.empty() || data.size() != number_of_items) {
        return E_INPUT_DATA_EMPTY;
    }
    if (data_query.empty()) {
        return E_CIPHERTEXT_REQUEST_EMPTY_OR_INVALID;
    }
    if (pir_server == nullptr) {
        return E_PIR_SERVER_ERROR;
    }

    int err_code = 0;
    // 对数据进行分块密态计算，每个密文块的大小为Seal_insert_max_size
    //获得data中最大元素的大小
    int max_size = 0;

    for (int i = 0; i < data.size(); ++i) {
        if (data[i].size() > max_size) {
            max_size = data[i].size();
        }
    }

    if (max_size == 0) {
        data_reply.push_back(vector<uint8_t>());
        return 0;
    }

    //对max_size除以Seal_insert_max_size向上取整
    int reply_number = ceil(static_cast<double>(max_size) / data_size);
    //将data_query分为两部分，第一部分为位置密文长度为32，第二部分为data_query
    data_reply.resize(reply_number);
    for (int j = 0; j < reply_number; ++j) {
        // data_block存储每个数据块，Seal_insert_max_size为每个数据块的大小
        std::vector<std::vector<uint8_t>> data_block(number_of_items);
        for (int i = 0; i < number_of_items; ++i) {
            if (data[i].size() > j * data_size && data[i].size() < (j + 1) * data_size) {
                data_block[i].assign(data[i].begin() + j * data_size, data[i].end());
            } else if (data[i].size() >= (j + 1) * data_size) {
                data_block[i].assign(data[i].begin() + j * data_size,
                                     data[i].begin() + (j + 1) * data_size);
            } else {
                continue;
            }
        }
        try {
            data_reply[j].clear();
            err_code = pir_server->generate_reply(data_query, data_block, data_reply[j], number_of_items, data_size);
            if (err_code != 0) {
                return err_code;
            }
        }
        catch (...) {
            return E_QUERY_ERROR;
        }
    }
    return 0;
}

int get_data_size(const std::vector<std::string> &data_file, std::vector<int> &data_size) {
    int max_data_size = 0;
    for (int i = 0; i < data_file.size(); ++i) {
        std::ifstream file(data_file[i], std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            return -1; // 文件打开失败
        }
        data_size[i] = file.tellg();
        if (max_data_size < file.tellg()) {
            max_data_size = file.tellg();
        }
        file.close();
    }
    return max_data_size;
}


bool
openInputFiles(std::string inputFile, std::ifstream *files[], int num, std::vector<int> &per_file_size,
               int &data_max_size) {
    for (int i = 0; i < num; i++) {
        std::string filePath = inputFile + std::to_string(i);
        std::ifstream *fs = new std::ifstream(filePath, std::ios::binary);
        if (!fs->is_open()) {
            return files; // Error opening file
        }
        files[i] = fs;
        struct stat st;
        stat(filePath.c_str(), &st);
        per_file_size[i] = st.st_size;
        if (st.st_size > data_max_size) {
            data_max_size = st.st_size;
        }
    }
    return true;
}

int PirServer::generate_reply_(int index_thread, int number_buff_per_thread, int mod_thread,
                               std::string inputFile, std::string outputFile,
                               const std::vector<uint8_t> &data_query, int &err) {
    // 确定文件读取的起始位置z
    std::vector<std::vector<uint8_t>> index;
    char buff[buff_size]; //读取数据的缓冲区，每次读取10000字节
    char lengthBuff[4]; //存储密文长度的部分，4字节

    int read_start_index = index_thread * number_buff_per_thread * buff_size;
    int write_start_index = index_thread * number_buff_per_thread * reply_max_size;
    std::vector<std::ifstream> files(number_of_items);
    for (int i = 0; i < number_of_items; i++) {
        std::string filePath = std::string(inputFile) + "/" + std::to_string(i);
        files[i].open(filePath, std::ios::in);
        if (!files[i].is_open()) {
            // 文件打开失败，记录日志并退出程序
            err = E_OPEN_FILE_FAILED;
            return err;
        }
        if (!files[i].seekg(read_start_index, std::ios::beg)) {
            err = E_SEEK_FILE_FAILED;
            files[i].close();
            return err;
        }
    }
    std::vector<std::vector<uint8_t>> reply(1);


    if (index_thread == number_buff_per_thread) {
        number_buff_per_thread += mod_thread;
    }

    std::ofstream outfile(outputFile, std::ios::out);


    std::vector<std::vector<uint8_t>> data_result(number_of_items);
    for (int i = 0; i < number_buff_per_thread; ++i) {
        if (!outfile.seekp(write_start_index, std::ios::beg)) {
            // seekp 失败，记录日志并退出程序
            err = E_SEEK_FILE_FAILED;
            return err;
        }
        for (int j = 0; j < number_of_items; ++j) {
            files[j].read(buff, buff_size);
            data_result[j].clear();
            data_result[j].insert(data_result[j].end(), buff, buff + files[j].gcount());
        }
        reply[0].clear();
        err = generate_data_reply(data_result, data_query, reply);
        if (err != 0) {
            std::cout << "C++ data_result" << data_result.size() << data_query.size() << std::endl;
            return err;
        }
        reply[0].resize(reply_max_size - 4); // 将reply的大小设置为reply_max_size(50000字节)

        uint32_t length = reply[0].size();
        if (reply[0].size() > reply_max_size) {
            err = E_REPLY_SIZE_ERROR;
        }

        uint32_t *iptr = reinterpret_cast<uint32_t *>(lengthBuff);
        *iptr = length;

        // 使用写入流操作符来写入数据
        outfile.write(reinterpret_cast<char *>(lengthBuff), 4);
        outfile.write(reinterpret_cast<char *>(reply[0].data()), length);
        write_start_index += reply_max_size;
    }
    outfile.close();
    for (int i = 0; i < number_of_items; ++i) {
        files[i].close();
    }

    return 0;


}


int PirServer::generate_reply_file(const std::string data_file, const std::vector<uint8_t> &data_query,
                                   const std::string reply_file, int number_threads) {

    if (number_threads < 1) {
        return E_NUMBER_THREADS_ERROR;
    }

    std::vector<std::vector<uint8_t>> dataResult(number_of_items);
    std::vector<int> per_file_size(number_of_items);

    // 打开输入文件
    std::ifstream *files[number_of_items];
    int data_max_size = 0;
    if (!openInputFiles(data_file, files, number_of_items, per_file_size, data_max_size)) {
        return E_OPEN_DATA_INPUT_FILE_FAILED;
    }
    if (data_query.empty()) {
        return E_CIPHERTEXT_REQUEST_EMPTY_OR_INVALID;
    }
    // 根据data_max_size将所有数据切分成若干个块，每个块的大小为10000字节
    int number_buff = std::ceil(static_cast<double>(data_max_size) / buff_size); // 一共有多少个块
    int number_buff_per_thread = number_buff / number_threads; // 每个线程处理多少个块, 多余的数据都给最后一个线程处理，微乎其微。
    int mod_thread = number_buff % number_threads; // 余数

    // 打开输出文件
    creat_path(reply_file);
    std::ofstream outfile(reply_file, std::ios::out);
    if (!outfile.is_open()) {
        return E_OPEN_REPLY_OUT_FILE_FAILED;
    }
    if (!outfile.seekp(number_buff * reply_max_size, std::ios::beg)) {
        // seekp 失败，记录日志并退出程序
        outfile.close();
        return E_SEEK_FILE_FAILED;
    }
    //关闭outfile
    outfile.close();
    for (int i = 0; i < number_of_items; i++) {
        if (files[i] && files[i]->is_open()) {
            files[i]->close();
            delete files[i];
        }
    }

    std::vector<int> errs(number_threads, 0);
    std::vector<std::thread> threads;
    for (int i = 0; i < number_threads; ++i) {
        threads.emplace_back(
                std::bind(&PirServer::generate_reply_, this, i, number_buff_per_thread, mod_thread, data_file,
                          reply_file, std::ref(data_query), std::ref(errs[i])));
    }

    for (auto &thread: threads) {
        thread.join();
    }
    for (auto er: errs) {
        if (er != 0) {
            return er;
        }
    }

    return 0;
}



