#pragma once

#include "pir.h"
#include "seal_client.h"
#include <map>
#include <memory>
#include <vector>


class PIRServer {
public:
    PIRServer(int number_of_items, int size_per_item);
    ~PIRServer(){
//        std::cout << "PIRServer destructor" << std::endl;
    };
    // NOTE: server takes over ownership of db and frees it when it exits.
    // Caller cannot free db
    void set_database(std::unique_ptr<std::vector<seal::Plaintext>> &&db);

    void set_database(const std::unique_ptr<const std::uint8_t[]> &bytes,
                      std::uint64_t ele_num, std::uint64_t ele_size);

    void set_data(const std::unique_ptr<const uint8_t[]> &bytes,
                  std::unique_ptr<Database> &db,
                  uint64_t ele_num, uint64_t ele_size);

    void preprocess_database();

    std::vector<seal::Ciphertext> expand_query(const seal::Ciphertext &encrypted,
                                               std::uint32_t m);

//  PirQuery deserialize_query(std::stringstream &stream);
    PirQuery deserialize_query(const std::vector<uint8_t> &query);

    int generate_reply(const std::vector<uint8_t> &query,
                       const std::vector<std::vector<uint8_t>> &data,
                       std::vector<uint8_t> &reply,
                       std::uint64_t ele_num, std::uint64_t ele_size);

    PirReply generate_reply(PirQuery &query, std::unique_ptr<Database> & db);
    // Serializes the reply into the provided stream and returns the number of
    // bytes written
    int serialize_reply(PirReply &reply, std::vector<uint8_t> &reply_char);

    void set_galois_key(const std::vector<uint8_t> &galois_keys);

    // Below simple operations are for interacting with the database WITHOUT PIR.
    // So they can be used to modify a particular element in the database or
    // to query a particular element (without privacy guarantees).
    void simple_set(std::uint64_t index, seal::Plaintext pt);

    seal::Ciphertext simple_query(std::uint64_t index);

    void set_one_ct(seal::Ciphertext one);

private:
    seal::EncryptionParameters enc_params_; // SEAL parameters
    PirParams pir_params_;                  // PIR parameters
    std::unique_ptr<Database> db_;
    bool is_db_preprocessed_;
    seal::GaloisKeys galoisKeys_;
    std::shared_ptr<seal::Evaluator> evaluator_;
    std::unique_ptr<seal::BatchEncoder> encoder_;
    std::shared_ptr<seal::SEALContext> context_;
    std::unique_ptr<seal::KeyGenerator> keygen_;
    // This is only used for simple_query
    seal::Ciphertext one_;

    void multiply_power_of_X(const seal::Ciphertext &encrypted,
                             seal::Ciphertext &destination, std::uint32_t index);
};
