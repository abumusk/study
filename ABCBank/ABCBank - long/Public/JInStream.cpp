#include "JInStream.h"

using namespace PUBLIC;

JInStream::JInStream()
	: currIndex_(0)
{
}

JInStream::JInStream(const char* data, size_t len)
	: currIndex_(0)
{
	SetData(data, len);
}

void JInStream::SetData(const char* data, size_t len)
{
	currIndex_ = 0;
	buffer_.resize(len);
	std::copy(data, data+len, buffer_.begin());
}

//stdext::checked iterator 确保你的操作不会越界(在你的容器中)。
//ptr  : 指向数组的指针。 (A pointer to the array.)
//size : 数组的大小。 ( The size of the array.)
//index: (任意)选取一个数组中的元素，初始化迭代器。默认使用数组的第一个元素。返回的迭代器
void JInStream::ReadBytes(void* data, size_t len)
{
	assert(ReadableBytes() >= len);
	std::copy(buffer_.begin()+currIndex_, 
		buffer_.begin()+currIndex_+len,
		stdext::checked_array_iterator<char*>(static_cast<char*>(data), len));
	currIndex_ += len;
}

JInStream& JInStream::operator >>(uint8& x)
{
	assert(ReadableBytes() >= sizeof(uint8));
	x = *Peek();
	currIndex_ += sizeof(x);

	return *this;
}

JInStream& JInStream::operator >>(uint16& x)
{
	assert(ReadableBytes() >= sizeof(uint16));
	uint16 be16 = 0;
	::memcpy(&be16, Peek(), sizeof(be16));
	currIndex_ += sizeof(be16);

	x = Endian::NotworkToHost16(be16);

	return *this;
}

JInStream& JInStream::operator >>(uint32& x)
{
	assert(ReadableBytes() >= sizeof(uint32));
	uint32 be32 = 0;
	::memcpy(&be32, Peek(), sizeof(be32));
	currIndex_ += sizeof(be32);

	x = Endian::NotworkToHost32(be32);

	return *this;
}

JInStream& JInStream::operator >>(int8& x)
{
	assert(ReadableBytes() >= sizeof(int8));
	x = *Peek();
	currIndex_ += sizeof(x);

	return *this;
}

JInStream& JInStream::operator >>(int16& x)
{
	assert(ReadableBytes() >= sizeof(int16));
	int16 be16 = 0;
	::memcpy(&be16, Peek(), sizeof(be16));
	currIndex_ += sizeof(be16);

	x = Endian::NotworkToHost16(be16);

	return *this;
}

JInStream& JInStream::operator >>(int32& x)
{
	assert(ReadableBytes() >= sizeof(int32));
	int32 be32 = 0;
	::memcpy(&be32, Peek(), sizeof(be32));
	currIndex_ += sizeof(be32);

	x = Endian::NotworkToHost32(be32);

	return *this;
}

JInStream& JInStream::operator >>(std::string& str)
{
	uint16 len;
	*this>>len;
	assert(ReadableBytes() >= len);
	str.clear();
	str.append(Peek(), len);
	currIndex_ += len;

	return *this;
}