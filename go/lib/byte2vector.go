package pir

func ByteToVectorUInt8(bytes []byte) VectorUInt8 {
	var i int64
	res := NewVectorUInt8()
	for i = 0; i < int64(len(bytes)); i++ {
		res.Add(bytes[i])
	}
	return res
}
func VectorUInt8ToBytes(vector VectorUInt8) []byte {
	var i int64
	var res []byte
	for i = 0; i < vector.Size(); i++ {
		data := vector.Get(int(i))
		res = append(res, data)
	}
	return res
}

func BytesToArrayOfVectorUInt8(bytes [][]byte) ArrayofVectorUInt8 {
	VectorList := NewArrayofVectorUInt8()
	for i := 0; i < len(bytes); i++ {
		res := ByteToVectorUInt8(bytes[i])
		VectorList.Add(res)
	}
	return VectorList
}
func ArrayOfVectorUInt8ToBytes(vector ArrayofVectorUInt8) [][]byte {
	var i int64
	var res [][]byte
	for i = 0; i < vector.Size(); i++ {
		data := VectorUInt8ToBytes(vector.Get(int(i)))
		res = append(res, data)
	}
	return res
}
