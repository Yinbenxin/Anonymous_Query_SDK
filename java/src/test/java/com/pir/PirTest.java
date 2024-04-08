package com.pir;

import org.junit.Test;

import java.util.Random;

public class PirTest {
    static {
        System.load("/Users/admin/git/Pir-SDK/java/src/main/resources/libpir-java-arm64.dylib");
    }
    @Test
    public void pirTest() {
        int numberItems = 5;
        int sizePerItem = 10000;
        int index = 3;
        boolean isCheck = false;
        byte[][] data = new byte[numberItems][sizePerItem];
        Random rand = new Random();

        // 填充二维字节数组
        for (int i = 0; i < numberItems; i++) {
            for (int j = 0; j < sizePerItem; j++) {
                data[i][j] = (byte) rand.nextInt(256); // 生成0-255之间的随机字节
            }
        }
        ArrayofVectorUInt8 dataVector = Vector2Byte.BytesToArrayOfVectorUInt8(data);

        // 创建一个空的 VectorUInt8 对象
        VectorUInt8 vector = new VectorUInt8();
        ArrayofVectorUInt8 IDVector = new ArrayofVectorUInt8();
        ArrayofVectorUInt8 query = new ArrayofVectorUInt8();
        ArrayofVectorUInt8 reply = new ArrayofVectorUInt8();
        VectorUInt8 plaintext = new VectorUInt8();
        VectorUInt8 keyClient = new VectorUInt8();
        SWIGTYPE_p_int p1 = pir.new_intp();
        PirClient client = new PirClient(numberItems, keyClient, isCheck, p1);
        if (pir.intp_value(p1) != 0) {
            System.out.println("测试失败，错误码：" + pir.intp_value(p1));
        }
        VectorUInt8 publicKey = client.get_public_key();
        PirServer server = new PirServer(numberItems, publicKey, isCheck, p1);
        if (pir.intp_value(p1) != 0) {
            System.out.println("测试失败，错误码：" + pir.intp_value(p1));
        }
        pir.delete_intp(p1);

        long start = System.currentTimeMillis();

        int err = client.generate_data_query(IDVector, index, query);
        if (err != 0) {
            System.out.println("测试失败，错误码：" + err);
        }

        err = server.generate_data_reply(dataVector, IDVector, query, reply);
        if (err != 0) {
            System.out.println("测试失败，错误码：" + err);
        }

        err = client.decrypt_data_reply(reply, plaintext);
        if (err != 0) {
            System.out.println("测试失败，错误码：" + err);
        }
        byte[] plaintexts = Vector2Byte.VectorUInt8ToBytes(plaintext);
        long end = System.currentTimeMillis();

        for (int i = 0; i < plaintexts.length; i++) {
            if (plaintexts[i] != data[index][i]) {
                System.out.println("测试失败，错误位置：" + i);
                break;
            }
        }
        System.out.println("测试成功！");
        System.out.println("耗时：" + (end - start) + "毫秒");

    }


}
