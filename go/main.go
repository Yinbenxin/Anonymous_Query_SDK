package main

import (
	"fmt"
	"gitlab.yunzhen-data.com/SharedComputing/Pir-SDK/go/lib"
	"math/rand"
	"time"
)

func main() {

	numberItems := 5
	sizePerItem := 10000
	index := 3
	isCheck := false
	faker := pir.NewFaker()

	Ids := faker.GenerateCNIDCardNumber(sizePerItem)
	IdsByte := pir.ArrayOfVectorUInt8ToBytes(Ids)

	fmt.Println(IdsByte[0])

	data := make([][]byte, numberItems)
	for i := 0; i < numberItems; i++ {
		data[i] = make([]byte, sizePerItem)
	}
	// 填充二维字节切片
	for i := 0; i < numberItems; i++ {
		for j := 0; j < sizePerItem; j++ {
			data[i][j] = byte(rand.Intn(256)) // 生成0-255之间的随机字节
		}
	}

	dataVector := pir.BytesToArrayOfVectorUInt8(data)

	IDVector := pir.NewArrayofVectorUInt8()
	query := pir.NewArrayofVectorUInt8()
	reply := pir.NewArrayofVectorUInt8()
	plaintext := pir.NewVectorUInt8()
	keyClient := pir.NewVectorUInt8()

	var errCode int
	client := pir.NewPirClient(numberItems, keyClient, isCheck, &errCode)

	publicKey := client.Get_public_key()
	server := pir.NewPirServer(numberItems, publicKey, isCheck, &errCode)

	start := time.Now()

	errCode = client.Generate_data_query(IDVector, index, query)
	if errCode != 0 {
		fmt.Println("测试失败，错误码：", errCode)
	}

	errCode = server.Generate_data_reply(dataVector, IDVector, query, reply)
	if errCode != 0 {
		fmt.Println("测试失败，错误码：", errCode)
	}

	errCode = client.Decrypt_data_reply(reply, plaintext)
	if errCode != 0 {
		fmt.Println("测试失败，错误码：", errCode)
	}
	plaintexts := pir.VectorUInt8ToBytes(plaintext)
	end := time.Now()

	for i := 0; i < len(plaintexts); i++ {
		if plaintexts[i] != data[index][i] {
			fmt.Println("测试失败，错误位置：", i)
			break
		}
	}
	fmt.Println("测试成功！")
	fmt.Println("耗时：", end.Sub(start))

}
