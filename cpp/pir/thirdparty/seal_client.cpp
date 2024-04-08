#include "pir.h"
#include "seal_client.h"
using namespace std;
using namespace seal;
using namespace seal::util;

PIRClient::PIRClient(int number_of_items, int size_per_item){
    EncryptionParameters enc_params(scheme_type::bfv);
    PirParams pir_params;
    gen_encryption_params(Poly_degree, Logt, enc_params);
    //当总数据量小于1024*1024时d=1，此时通信开销较小，当总数据量大于1024*1024时d=2，此时计算开销较小。
    uint32_t d = number_of_items * size_per_item > 1 << 20 ? 2 : 1;

    gen_pir_params(number_of_items, size_per_item, d, enc_params, pir_params,
                   use_symmetric, use_batching, use_recursive_mod_switching);
    context_ = make_shared<SEALContext>(enc_params, true);  // 设置 context
    enc_params_ = enc_params;
    pir_params_ = pir_params;
    keygen_ = make_unique<KeyGenerator>(*context_);
}

void PIRClient::GenerateKey(vector<uint8_t> &secret_key, vector<uint8_t> &galois_key) {
    SecretKey s_key = keygen_->secret_key();
    GaloisKeys g_key = generate_galois_keys();
    if (pir_params_.enable_symmetric) {
        encryptor_ = make_unique<Encryptor>(*context_, s_key);
    }
    decryptor_ = make_unique<Decryptor>(*context_, s_key);
    evaluator_ = make_unique<Evaluator>(*context_);
    encoder_ = make_unique<BatchEncoder>(*context_);
    std::stringstream sk_stream;
    std::stringstream gk_stream;
    s_key.save(sk_stream);
    g_key.save(gk_stream);
    //stringstream sk_stream转为vector<uint8_t>secret_key
    secret_key.assign(std::istreambuf_iterator<char>(sk_stream), std::istreambuf_iterator<char>());
    galois_key.assign(std::istreambuf_iterator<char>(gk_stream), std::istreambuf_iterator<char>());
    sk_stream.clear();
    gk_stream.clear();

}

void PIRClient::GenerateQuery(std::vector<uint8_t> &query, int index) {
    std::stringstream qy_stream;
    uint64_t ele_index = index;

    uint64_t index_fv = get_fv_index(ele_index);
    generate_serialized_query(index_fv, qy_stream);
    query.assign(std::istreambuf_iterator<char>(qy_stream), std::istreambuf_iterator<char>());

    qy_stream.clear();
}

void PIRClient::loadKey(const vector<uint8_t> &secret_key) {
    SecretKey s_key;
    std::stringstream sk_stream;
    sk_stream.write((char *) secret_key.data(), secret_key.size());
    s_key.load(*context_, sk_stream);
    if (pir_params_.enable_symmetric) {
        encryptor_ = make_unique<Encryptor>(*context_, s_key);
    }
    decryptor_ = make_unique<Decryptor>(*context_, s_key);
    evaluator_ = make_unique<Evaluator>(*context_);
    encoder_ = make_unique<BatchEncoder>(*context_);
    sk_stream.clear();
}

int PIRClient::generate_serialized_query(uint64_t desiredIndex,
                                         std::stringstream &stream) {

    int N = enc_params_.poly_modulus_degree();
    int output_size = 0;
    auto indices = compute_indices(desiredIndex, pir_params_.nvec);
    Plaintext pt(enc_params_.poly_modulus_degree());

    for (uint32_t i = 0; i < indices.size(); i++) {
        uint32_t num_ptxts = ceil((pir_params_.nvec[i] + 0.0) / N);


        for (uint32_t j = 0; j < num_ptxts; j++) {
            pt.set_zero();
            if (indices[i] >= N * j && indices[i] <= N * (j + 1)) {
                uint64_t real_index = indices[i] - N * j;
                uint64_t n_i = pir_params_.nvec[i];
                uint64_t total = N;
                if (j == num_ptxts - 1) {
                    total = n_i % N;
                }
                uint64_t log_total = ceil(log2(total));

//        cout << "Client: Inverting " << pow(2, log_total) << endl;
                pt[real_index] =
                        invert_mod(pow(2, log_total), enc_params_.plain_modulus());
            }

            if (pir_params_.enable_symmetric) {
                output_size += encryptor_->encrypt_symmetric(pt).save(stream);
            } else {
                output_size += encryptor_->encrypt(pt).save(stream);
            }
        }
    }

    return output_size;
}

PirQuery PIRClient::generate_query(uint64_t desiredIndex) {
    auto indices = compute_indices(desiredIndex, pir_params_.nvec);

    PirQuery result(pir_params_.d);
    int N = enc_params_.poly_modulus_degree();

    Plaintext pt(enc_params_.poly_modulus_degree());
    for (uint32_t i = 0; i < indices.size(); i++) {
        uint32_t num_ptxts = ceil((pir_params_.nvec[i] + 0.0) / N);
        // initialize result.
//    cout << "Client: index " << i + 1 << "/ " << indices.size() << " = "
//         << indices[i] << endl;
//    cout << "Client: number of ctxts needed for query = " << num_ptxts << endl;

        for (uint32_t j = 0; j < num_ptxts; j++) {
            pt.set_zero();
            if (indices[i] >= N * j && indices[i] <= N * (j + 1)) {
                uint64_t real_index = indices[i] - N * j;
                uint64_t n_i = pir_params_.nvec[i];
                uint64_t total = N;
                if (j == num_ptxts - 1) {
                    total = n_i % N;
                }
                uint64_t log_total = ceil(log2(total));

//        cout << "Client: Inverting " << pow(2, log_total) << endl;
                pt[real_index] =
                        invert_mod(pow(2, log_total), enc_params_.plain_modulus());
            }
            Ciphertext dest;
            if (pir_params_.enable_symmetric) {
                encryptor_->encrypt_symmetric(pt, dest);
            } else {
                encryptor_->encrypt(pt, dest);
            }
            result[i].push_back(dest);
        }
    }

    return result;
}

//
uint64_t PIRClient::get_fv_index(uint64_t element_index) {
    return static_cast<uint64_t>(element_index /
                                 pir_params_.elements_per_plaintext);
}

uint64_t PIRClient::get_fv_offset(uint64_t element_index) {
    return element_index % pir_params_.elements_per_plaintext;
}

Plaintext PIRClient::decrypt(Ciphertext ct) {
    Plaintext pt;
    decryptor_->decrypt(ct, pt);
    return pt;
}

//vector<uint8_t> PIRClient::decode_reply(PirReply &reply, uint64_t offset) {
//    std::lock_guard<std::mutex> lock(mtx1);//加锁
//    Plaintext result = decode_reply(reply);
//    return extract_bytes(result, offset);
//}

vector<uint64_t> PIRClient::extract_coeffs(Plaintext pt) {
    vector<uint64_t> coeffs;
    encoder_->decode(pt, coeffs);
    return coeffs;
}

std::vector<uint64_t> PIRClient::extract_coeffs(seal::Plaintext pt,
                                                uint64_t offset) {
    vector<uint64_t> coeffs;
    encoder_->decode(pt, coeffs);

    uint32_t logt = floor(log2(enc_params_.plain_modulus().value()));

    uint64_t coeffs_per_element =
            coefficients_per_element(logt, pir_params_.ele_size);

    return std::vector<uint64_t>(coeffs.begin() + offset * coeffs_per_element,
                                 coeffs.begin() +
                                 (offset + 1) * coeffs_per_element);
}

std::vector<uint8_t> PIRClient::extract_bytes(seal::Plaintext pt,
                                              uint64_t offset) {
    uint32_t logt = floor(log2(enc_params_.plain_modulus().value()));
    uint32_t bytes_per_ptxt =
            pir_params_.elements_per_plaintext * pir_params_.ele_size;

    // Convert from FV plaintext (polynomial) to database element at the client
    vector<uint8_t> elems(bytes_per_ptxt);
    vector<uint64_t> coeffs;
//    auto encoder = encoder_;

    encoder_->decode(pt, coeffs);
    coeffs_to_bytes(logt, coeffs, elems.data(), bytes_per_ptxt,
                    pir_params_.ele_size);
    return std::vector<uint8_t>(elems.begin() + offset * pir_params_.ele_size,
                                elems.begin() +
                                (offset + 1) * pir_params_.ele_size);
}

vector<uint8_t> PIRClient::decode_reply(PirReply &reply, uint64_t offset) {
    EncryptionParameters parms;
    parms_id_type parms_id;

    if (pir_params_.enable_mswitching) {
        parms = context_->last_context_data()->parms();
        parms_id = context_->last_parms_id();
    } else {
        parms = context_->first_context_data()->parms();
        parms_id = context_->first_parms_id();
    }
    uint32_t exp_ratio = compute_expansion_ratio(parms);
    uint32_t recursion_level = pir_params_.d;

    vector<Ciphertext> temp = reply;
    uint32_t ciphertext_size = temp[0].size();

    uint64_t t = enc_params_.plain_modulus().value();
    ;

    for (uint32_t i = 0; i < recursion_level; i++) {
//    cout << "Client: " << i + 1 << "/ " << recursion_level
//         << "-th decryption layer started." << endl;
        vector<Ciphertext> newtemp;
        vector<Plaintext> tempplain;

        for (uint32_t j = 0; j < temp.size(); j++) {
            Plaintext ptxt;
            decryptor_->decrypt(temp[j], ptxt);
#ifdef DEBUG
            cout << "Client: reply noise budget = "
                 << decryptor_->invariant_noise_budget(temp[j]) << endl;
#endif

            // cout << "decoded (and scaled) plaintext = " << ptxt.to_string() <<
            // endl;
            tempplain.push_back(ptxt);

#ifdef DEBUG
            cout << "recursion level : " << i << " noise budget :  ";
            cout << decryptor_->invariant_noise_budget(temp[j]) << endl;
#endif

            if ((j + 1) % (exp_ratio * ciphertext_size) == 0 && j > 0) {
                // Combine into one ciphertext.
                Ciphertext combined(*context_, parms_id);
                compose_to_ciphertext(parms, tempplain, combined);
                newtemp.push_back(combined);
                tempplain.clear();
                // cout << "Client: const term of ciphertext = " << combined[0] << endl;
            }
        }
//    cout << "Client: done." << endl;
//    cout << endl;
        if (i == recursion_level - 1) {
            assert(temp.size() == 1);
            return extract_bytes(tempplain[0], offset);

        } else {
            tempplain.clear();
            temp = newtemp;
        }
    }

    // This should never be called
    assert(0);
    Plaintext fail;
    return extract_bytes(fail, offset);
}

GaloisKeys PIRClient::generate_galois_keys() {
    // Generate the Galois keys needed for coeff_select.
    vector<uint32_t> galois_elts;
    int N = enc_params_.poly_modulus_degree();
    int logN = get_power_of_two(N);

    // cout << "printing galois elements...";
    for (int i = 0; i < logN; i++) {
        galois_elts.push_back((N + exponentiate_uint(2, i)) /
                              exponentiate_uint(2, i));
        //#ifdef DEBUG
        // cout << galois_elts.back() << ", ";
        //#endif
    }
    GaloisKeys gal_keys;
    keygen_->create_galois_keys(galois_elts, gal_keys);
    return gal_keys;
}

Plaintext PIRClient::replace_element(Plaintext pt, vector<uint64_t> new_element,
                                     uint64_t offset) {
    vector<uint64_t> coeffs = extract_coeffs(pt);

    uint32_t logt = floor(log2(enc_params_.plain_modulus().value()));
    uint64_t coeffs_per_element =
            coefficients_per_element(logt, pir_params_.ele_size);

    assert(new_element.size() == coeffs_per_element);

    for (uint64_t i = 0; i < coeffs_per_element; i++) {
        coeffs[i + offset * coeffs_per_element] = new_element[i];
    }

    Plaintext new_pt;

    encoder_->encode(coeffs, new_pt);
    return new_pt;
}

Ciphertext PIRClient::get_one() {
    Plaintext pt("1");
    Ciphertext ct;
    if (pir_params_.enable_symmetric) {
        encryptor_->encrypt_symmetric(pt, ct);
    } else {
        encryptor_->encrypt(pt, ct);
    }
    return ct;
}


PirReply PIRClient::deserialize_reply(const std::vector<uint8_t> &reply) {
    PirReply q;
    stringstream stream;
    stream.write((char *) reply.data(), reply.size());
    int a = pir_params_.d == 1 ? 1 : 4;
    vector<Ciphertext> cs;
    for (uint32_t j = 0; j < a; j++) {
        Ciphertext c;
        c.load(*context_, stream);
        cs.push_back(c);
    }
    stream.clear();
    return cs;
}