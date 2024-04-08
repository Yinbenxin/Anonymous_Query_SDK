/**
 * @file   errorcode.h
 * @author admin
 * @date   2023.11.23
 *
 * @brief
 *         通用错误码定义及实现
 *
 * @note
 *         1、错误码从0开始计算，顺序添加。
 *         2、实际使用时，除E_OK外，错误码可以是负数(需要手工添加'-')，也可以是正数。
 *         3、错误码可以是任何数字，但不在枚举之列则显示未知错误码。
 *
 * @log
 *
 */

#ifndef PIR_ERRORCODE_H
#define PIR_ERRORCODE_H


enum ErrorCode{
    E_OK,                                           ///< 成功0
    E_FAIL,                                         ///< 一般性错误1
    E_ALGORITHM_ERROR,                              ///< 算法错误2
    E_NUMBER_OR_DATA_SIZE_INVALID,                  ///< 匿名倍数或数据宽度错误3
    E_INIT_PARAMETER_ERROR,                         ///< 位置参数错误4
    E_PRIVATE_KEY_ERROR,                            ///< 私钥错误5
    E_PUBLIC_KEY_ERROR,                             ///< 公钥错误6
    E_PIR_CLIENT_ERROR,                             ///< pir_client参数为空7
    E_PIR_SERVER_ERROR,                             ///< pir_server参数为空8
    E_INDEX_ERROR,                                  ///< 索引为空9
    E_DATA_REPLY_EMPTY_OR_INVALID,                  ///< data_reply为空或错误10
    E_INPUT_DATA_EMPTY,                             ///< 输入数据为空11
    E_INPUT_DATA_EXCEED_MAX_VALUE,                  ///< 输入数据超过设定最大值12
    E_CIPHERTEXT_REQUEST_EMPTY_OR_INVALID,          ///< 密文请求为空或错误13
    E_DATA_LENGTH_INDEX_COUNT_MISMATCH,             ///< 数据长度和索引个数不同14
    E_CIPHERTEXT_START_POSITION_ERROR,              ///< 密文起始位置错误15
    E_CIPHERTEXT_END_POSITION_ERROR,                ///< 密文结束位置错误16
    E_UNKNOWN_OPRF_ALGORITHM,                       ///< OPRF算法未知17
    E_ECC_KEY_LENGTH_ERROR,                         ///< ECC密钥错误长度不为32 18
    E_ECC_VALUE_MULTIPLICATION_INPUT_EMPTY,         ///< ECC值乘入参value为空19
    E_ECC_POINT_MULTIPLICATION_VALUE_LENGTH_ERROR,  ///< ECC点乘入参值长度错误不为32位20
    E_VALUE_TO_ECC_POINT_FALSE,                     ///< 值转换成点失败 21
    E_GENERATE_ENCRYPTION_KEY_FAILED,               ///< 生成加密密钥失败 22
    E_GENERATE_IV_FAILED,                           ///< 生成加密IV失败 23
    E_GENERATE_REPLY,                               ///< 生成reply失败 24
    E_CREATE_CIPHERTEXT_CONTENT_FAILED,             ///< 创建密文内容失败 25
    E_ENCRYPTOR_INITIALIZATION_FAILED,              ///< 加密器初始化失败 26
    E_ENCRYPT_DATA_FAILED,                          ///< 加密数据失败 27
    E_DECRYPT_DATA_FAILED,                          ///< 解密数据失败 28
    E_ENCRYPTION_END_FAILED,                        ///< 加密结束失败 29
    E_DECRYPTION_END_FAILED,                        ///< 解密结束失败 30
    E_QUERY_ERROR,                                  ///< query错误 31
    E_REPLY_ERROR,                                  ///< query错误 32
    E_OPEN_FILE_FAILED,                             ///< 打开文件失败 33
    E_INPUT_KEY_ERROR,                              ///< 输入密钥错误 34
    E_OUTPUT_FILE_ERROR,                            ///< 输出文件错误 35
    E_FAKER_TYPE_ERROR,                             ///< 伪造类型错误 36
    E_DATA_HEAD_NUMBERS_ERROR,                      ///< 数据头部数量错误 37
    E_DATA_HEAD_POSITION_ERROR,                     ///< 数据头部位置错误 38
    E_BUFFER_NOY_ENOUGH,                            ///< 缓冲区长度错误 39
    E_INPUT_DATA_ERROR,                             ///< 输入数据错误 40
    E_SERVER_NUMBERS_ERROR,                         ///< 输入数据长度错误 41
    E_INPUT_FILE_ERROR,                             ///< 输入文件错误 42
    E_OPEN_PUBLIC_KEY_FILE_FAILED,                  ///< 打开公钥文件失败 43
    E_OPEN_REPLY_OUT_FILE_FAILED,                   ///< 打开reply输出文件失败 44
    E_OPEN_DATA_INPUT_FILE_FAILED,                  ///< 打开数据输入文件失败 45
    E_OPEN_REPLY_INPUT_FILE_FAILED,                 ///< 打开reply输入文件失败 46
    E_OPEN_PLAINT_OUT_FILE_FAILED,                  ///< 打开plainy输出文件失败 47
    E_SEEK_FILE_FAILED,                             ///< 文件指针定位失败 48
    E_NUMBER_THREADS_ERROR,                         ///< 线程数量错误 49
    E_REPLY_SIZE_ERROR,                             ///< reply大小错误 50
    E_READ_FILE_ERROR,                              ///< 读取文件错误 51
    E_WRITE_FILE_ERROR,                             ///< 写入文件错误 52
    E_END,                                          ///< 占位，无实际作用
};


#endif //PIR_ERRORCODE_H
