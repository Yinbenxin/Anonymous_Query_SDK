package com.pir;

import org.junit.Test;


public class FakerTest {
    static {
        System.load("/Users/admin/git/Pir-SDK/java/src/main/resources/libpir-java-arm64.dylib");
    }
    @Test
    public void testFaker() {
        int sizePerItem = 10000;
        long start = System.currentTimeMillis();
        Faker faker = new Faker();
        ArrayofVectorUInt8 ids = faker.GenerateCNIDCardNumber(sizePerItem);
        byte[][] idsByte = Vector2Byte.ArrayOfVectorUInt8ToBytes(ids);
        long end = System.currentTimeMillis();
        System.out.println(idsByte.length);
        for (byte[] idByte : idsByte) {
            System.out.println(new String(idByte));
        }
        System.out.println("测试成功！");
        System.out.println("耗时：" + (end - start) + " 毫秒");
    }



}
