#pragma once
#include <cstddef>
#include <string>
#include "byte_order.h"
namespace basic{
enum Endianness{
    NETWORK_ORDER, //big
    HOST_ORDER,//little
};
class DataReader{
public:
    DataReader(const char* buf,uint32_t len);
    DataReader(const char* buf,uint32_t len,Endianness endianness);
    ~DataReader(){}
    bool ReadUInt8(uint8_t *result);
    bool ReadUInt16(uint16_t *result);
    bool ReadUInt32(uint32_t *result);
    bool ReadUInt64(uint64_t *result);
    bool ReadBytesToUInt64(uint32_t num_len,uint64_t *result);
    bool ReadBytes(void*result,uint32_t size);
    bool ReadUFloat16(uint64_t* result);
    bool ReadStringPiece16(std::string * result);
    bool ReadStringPiece(std::string * result, size_t size);
    bool IsDoneReading() const ;
    size_t BytesRemaining() const;
private:
    bool CanRead(uint32_t bytes);
    void OnFailure();
    const char *data_{0};
    const uint32_t len_{0};
    uint32_t pos_{0};
    Endianness endianness_{HOST_ORDER};
};
class DataWriter{
public:
    DataWriter(char* buf,uint32_t len);
    DataWriter(char* buf,uint32_t len,Endianness endianness);
    ~DataWriter(){}
    uint32_t length(){
        return pos_;
    }
    uint32_t capacity(){
        return capacity_;
    }
    bool WriteUInt8(uint8_t value);
    bool WriteUInt16(uint16_t value);
    bool WriteUInt32(uint32_t value);
    bool WriteUInt64(uint64_t value);
    bool WriteBytesToUInt64(uint32_t num_bytes, uint64_t value);
    bool WriteBytes(const void *value,uint32_t size);
    bool WriteUFloat16(uint64_t value);
private:
    char* BeginWrite(uint32_t bytes);
    char *buf_{0};
    uint32_t pos_{0};
    uint32_t capacity_{0};
    Endianness endianness_{HOST_ORDER};
};
}