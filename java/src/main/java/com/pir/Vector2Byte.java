package com.pir;

import java.util.ArrayList;

public class Vector2Byte {
    public static VectorUInt8 ByteToVectorUInt8(byte[] bytes) {
        VectorUInt8 res = new VectorUInt8();
        for (int i = 0; i < bytes.length; i++) {
            res.add((short) bytes[i]);
        }
        return res;
    }

    public static byte[] VectorUInt8ToBytes(VectorUInt8 vector) {
        byte[] res = new byte[vector.size()];
        for (int i = 0; i < vector.size(); i++) {
            res[i] = vector.get(i).byteValue();
        }
        return res;
    }

    public static ArrayofVectorUInt8 BytesToArrayOfVectorUInt8(byte[][] bytes) {
        ArrayofVectorUInt8 vectorList = new ArrayofVectorUInt8();
        for (int i = 0; i < bytes.length; i++) {
            VectorUInt8 res = ByteToVectorUInt8(bytes[i]);
            vectorList.add(res);
        }
        return vectorList;
    }

    public static byte[][] ArrayOfVectorUInt8ToBytes(ArrayofVectorUInt8 vector) {
        ArrayList<byte[]> res = new ArrayList<>();
        for (int i = 0; i < vector.size(); i++) {
            byte[] data = VectorUInt8ToBytes(vector.get(i));
            res.add(data);
        }
        return res.toArray(new byte[0][]);
    }
}
