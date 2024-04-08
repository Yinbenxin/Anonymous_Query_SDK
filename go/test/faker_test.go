package test

import (
	"fmt"
	"gitlab.yunzhen-data.com/SharedComputing/Pir-SDK/go/lib"
	"testing"
	"time"
)

func TestFaker(t *testing.T) {

	sizePerItem := 10000
	start := time.Now()
	faker := pir.NewFaker()
	Ids := faker.GenerateCNIDCardNumber(sizePerItem)
	IdsByte := pir.ArrayOfVectorUInt8ToBytes(Ids)
	end := time.Now()
	fmt.Println(len(IdsByte))
	for i := 0; i < len(IdsByte); i++ {
		fmt.Println(string(IdsByte[i]))
	}
	fmt.Println("测试成功！")
	fmt.Println("耗时：", end.Sub(start))

}
