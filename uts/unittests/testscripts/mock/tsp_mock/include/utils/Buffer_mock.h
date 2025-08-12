class MockBuffer {
    public:
    MOCK_METHOD2(setTo, void(const uint8_t* buf,const int32_t &len));
    MOCK_METHOD0(data, uint8_t*());
    MOCK_CONST_METHOD0(size, uint32_t());
    MOCK_CONST_METHOD0(empty, bool());
    MOCK_METHOD2(append, void(uint8_t* buf, int32_t len));
};

MockBuffer* M_Buffer;
void Buffer::setTo(const uint8_t* buf,const int32_t &len)
{
    M_Buffer->setTo(buf, len);
}

uint8_t* Buffer::data()
{
    return M_Buffer->data();
}

uint32_t Buffer::size() const
{
    return M_Buffer->size();
}

void Buffer::append(uint8_t* buf, int32_t len)
{
    M_Buffer->append(buf, len);
}

bool Buffer::empty() const
{
    return M_Buffer->empty();
}