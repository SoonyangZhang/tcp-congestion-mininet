#include <memory.h>
#include <limits>
#include <stdint.h>
#include "byte_codec.h"
#include "dmlc_logging.h"
namespace basic{
namespace {
// We define an unsigned 16-bit floating point value, inspired by IEEE floats
// (http://en.wikipedia.org/wiki/Half_precision_floating-point_format),
// with 5-bit exponent (bias 1), 11-bit mantissa (effective 12 with hidden
// bit) and denormals, but without signs, transfinites or fractions. Wire format
// 16 bits (little-endian byte order) are split into exponent (high 5) and
// mantissa (low 11) and decoded as:
//   uint64_t value;
//   if (exponent == 0) value = mantissa;
//   else value = (mantissa | 1 << 11) << (exponent - 1)
const int kUFloat16ExponentBits = 5;
const int kUFloat16MaxExponent = (1 << kUFloat16ExponentBits) - 2;     // 30
const int kUFloat16MantissaBits = 16 - kUFloat16ExponentBits;          // 11
const int kUFloat16MantissaEffectiveBits = kUFloat16MantissaBits + 1;  // 12
const uint64_t kUFloat16MaxValue =  // 0x3FFC0000000
    ((UINT64_C(1) << kUFloat16MantissaEffectiveBits) - 1)
    << kUFloat16MaxExponent;
}
DataReader::DataReader(const char* buf,uint32_t len)
:DataReader(buf,len,NETWORK_ORDER){}
DataReader::DataReader(const char* buf,uint32_t len,Endianness endianness)
:data_(buf)
,len_(len)
,pos_(0)
,endianness_(endianness){
}
bool DataReader::ReadUInt8(uint8_t *result){
    return ReadBytes(result, sizeof(uint8_t));
}
bool DataReader::ReadUInt16(uint16_t *result){
    if(!ReadBytes(result,sizeof(uint16_t))){
        return false;
    }
    if(endianness_ == NETWORK_ORDER){
        *result=basic::NetToHost16(*result);
    }
    return true;
}
bool DataReader::ReadUInt32(uint32_t *result){
    if(!ReadBytes(result,sizeof(uint32_t))){
        return false;
    }
    if(endianness_ == NETWORK_ORDER){
        *result=basic::NetToHost32(*result);
    }
    return true;
}
bool DataReader::ReadUInt64(uint64_t *result){
    if(!ReadBytes(result,sizeof(uint64_t))){
        return false;
    }
    if(endianness_ == NETWORK_ORDER){
        *result=basic::NetToHost64(*result);
    }
    return true;
}
bool DataReader::ReadBytesToUInt64(uint32_t num_len,uint64_t *result){
    *result=0u;
    if(HOST_ORDER==endianness_){
        return ReadBytes(result,num_len);
    }
    if(!ReadBytes(reinterpret_cast<char*>(result) + sizeof(*result) - num_len,num_len)){
        return false;
    }
    *result=basic::NetToHost64(*result);
    return true;
}
bool DataReader::ReadBytes(void*result,uint32_t size){
    if(!CanRead(size)){
        OnFailure();
        return false;
    }
    memcpy(result,data_+pos_,size);
    pos_+=size;
    return true;
}
bool DataReader::ReadUFloat16(uint64_t* result){
  uint16_t value;
  if (!ReadUInt16(&value)) {
    return false;
  }

  *result = value;
  if (*result < (1 << kUFloat16MantissaEffectiveBits)) {
    // Fast path: either the value is denormalized (no hidden bit), or
    // normalized (hidden bit set, exponent offset by one) with exponent zero.
    // Zero exponent offset by one sets the bit exactly where the hidden bit is.
    // So in both cases the value encodes itself.
    return true;
  }

  uint16_t exponent =
      value >> kUFloat16MantissaBits;  // No sign extend on uint!
  // After the fast pass, the exponent is at least one (offset by one).
  // Un-offset the exponent.
  --exponent;
  DCHECK_GE(exponent, 1);
  DCHECK_LE(exponent, kUFloat16MaxExponent);
  // Here we need to clear the exponent and set the hidden bit. We have already
  // decremented the exponent, so when we subtract it, it leaves behind the
  // hidden bit.
  *result -= exponent << kUFloat16MantissaBits;
  *result <<= exponent;
  DCHECK_GE(*result,
            static_cast<uint64_t>(1 << kUFloat16MantissaEffectiveBits));
  DCHECK_LE(*result, kUFloat16MaxValue);
  return true;
}
bool DataReader::ReadStringPiece16(std::string * result){
    uint16_t result_len=0;
    if(!ReadUInt16(&result_len)){
        return false;
    }
    return ReadStringPiece(result,result_len);
}
bool DataReader::ReadStringPiece(std::string * result, size_t size){
      // Make sure that we have enough data to read.
  if (!CanRead(size)) {
    OnFailure();
    return false;
  }
  *result=std::string(data_ + pos_, size);
  // Iterate.
  pos_ += size;
    return true;
}
bool DataReader::IsDoneReading() const {
  return len_ == pos_;
}
size_t DataReader::BytesRemaining() const {
  return len_ - pos_;
}
bool DataReader::CanRead(uint32_t bytes){
    return bytes<=(len_-pos_);
}
void DataReader::OnFailure(){
    pos_=len_;
}
DataWriter::DataWriter(char* buf,uint32_t len)
:DataWriter(buf,len,NETWORK_ORDER){
}
DataWriter::DataWriter(char* buf,uint32_t len,Endianness endianness)
:buf_(buf)
,pos_(0)
,capacity_(len)
,endianness_(endianness){
}
bool DataWriter::WriteUInt8(uint8_t value){
    return WriteBytes(&value,sizeof(uint8_t));
}
bool DataWriter::WriteUInt16(uint16_t value){
    if(endianness_ == NETWORK_ORDER){
        value=basic::HostToNet16(value);
    }
    return WriteBytes(&value,sizeof(uint16_t));
}
bool DataWriter::WriteUInt32(uint32_t value){
    if(endianness_ == NETWORK_ORDER){
        value=basic::HostToNet32(value);
    }
    return WriteBytes(&value,sizeof(uint32_t));
}
bool DataWriter::WriteUInt64(uint64_t value){
    if(endianness_ == NETWORK_ORDER){
        value=basic::HostToNet64(value);
    }
    return WriteBytes(&value,sizeof(uint64_t));
}
bool DataWriter::WriteBytesToUInt64(uint32_t num_bytes, uint64_t value){
    if(num_bytes>sizeof(value)){
        return false;
    }
    if(HOST_ORDER==endianness_){
        return WriteBytes(&value,num_bytes);
    }
    value=basic::HostToNet64(value);
    return WriteBytes(reinterpret_cast<char*>(&value)+sizeof(value)-num_bytes,num_bytes);
}
bool DataWriter::WriteBytes(const void *value,uint32_t size){
    char *dst=BeginWrite(size);
    if(!dst){
        return false;
    }
    memcpy((void*)dst,value,size);
    pos_+=size;
    return true;
}
bool DataWriter::WriteUFloat16(uint64_t value){
  uint16_t result;
  if (value < (UINT64_C(1) << kUFloat16MantissaEffectiveBits)) {
    // Fast path: either the value is denormalized, or has exponent zero.
    // Both cases are represented by the value itself.
    result = static_cast<uint16_t>(value);
  } else if (value >= kUFloat16MaxValue) {
    // Value is out of range; clamp it to the maximum representable.
    result = std::numeric_limits<uint16_t>::max();
  } else {
    // The highest bit is between position 13 and 42 (zero-based), which
    // corresponds to exponent 1-30. In the output, mantissa is from 0 to 10,
    // hidden bit is 11 and exponent is 11 to 15. Shift the highest bit to 11
    // and count the shifts.
    uint16_t exponent = 0;
    for (uint16_t offset = 16; offset > 0; offset /= 2) {
      // Right-shift the value until the highest bit is in position 11.
      // For offset of 16, 8, 4, 2 and 1 (binary search over 1-30),
      // shift if the bit is at or above 11 + offset.
      if (value >= (UINT64_C(1) << (kUFloat16MantissaBits + offset))) {
        exponent += offset;
        value >>= offset;
      }
    }

    DCHECK_GE(exponent, 1);
    DCHECK_LE(exponent, kUFloat16MaxExponent);
    DCHECK_GE(value, UINT64_C(1) << kUFloat16MantissaBits);
    DCHECK_LT(value, UINT64_C(1) << kUFloat16MantissaEffectiveBits);

    // Hidden bit (position 11) is set. We should remove it and increment the
    // exponent. Equivalently, we just add it to the exponent.
    // This hides the bit.
    result = static_cast<uint16_t>(value + (exponent << kUFloat16MantissaBits));
  }

  if (endianness_ == NETWORK_ORDER) {
    result = basic::HostToNet16(result);
  }
  return WriteBytes(&result, sizeof(result));
}
char* DataWriter::BeginWrite(uint32_t bytes){
    if(pos_>capacity_){
        return nullptr;
    }
    if(capacity_-pos_<bytes){
        return nullptr;
    }
    return buf_+pos_;
}
}
