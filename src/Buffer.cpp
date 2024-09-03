#include "../include/Buffer.h"

Buffer::Buffer()
{

}

Buffer::~Buffer()
{
    
}

void Buffer::append(const char *data, size_t size)
{
    buf_.append(data, size);
}

void Buffer::appendwithhead(const char *data, size_t size)
{
    buf_.append((char*)&size, 4);   // 先添加报文长度(头部)
    buf_.append(data, size);    //  处理报文内容
}

size_t Buffer::size()
{
    return buf_.size();
}

void Buffer::clear()
{
    buf_.clear();
}

// 从buf_的pos开始，删除i个字节，pos从0开始
void Buffer::erase(size_t pos, size_t i)
{
    buf_.erase(pos, i);
}

const char *Buffer::data()
{
    return buf_.data();
}
