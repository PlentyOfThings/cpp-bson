#ifndef POT_BSON_CONSTS_H_
#define POT_BSON_CONSTS_H_

#include <stdlib.h>

namespace pot {
namespace bson {

static constexpr size_t kIntKeySize = 12;

inline int convert_int_key_to_str(int32_t ikey, char skey[]) {
  return snprintf(skey, kIntKeySize, "%d", ikey);
}

enum struct TypeSize : uint8_t {
  Byte = 1,
  Int32 = 4,
  Int64 = 8,
  Uint64 = 8,
  Double = 8,
  Decimal128 = 16,
};

enum struct Element : uint8_t {
  Double = 0x01,
  String = 0x02, // can contain null chars
  Document = 0x03,
  Array = 0x04,
  Binary = 0x05,
  Boolean = 0x08,
  Null = 0x0A,
  Int32 = 0x10,
  Int64 = 0x12,
  Decimal128 = 0x13,
};

enum struct BinaryElementSubtype : uint8_t {
  Generic = 0x00,
};

enum struct BooleanElementValue : uint8_t {
  False = 0x00,
  True = 0x01,
};

} // namespace bson
} // namespace pot

#endif
