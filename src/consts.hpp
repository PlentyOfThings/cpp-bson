#ifndef POT_BSON_CONSTS_H_
#define POT_BSON_CONSTS_H_

#include "stdlib.h"

namespace pot {
namespace bson {

enum struct kTypeSize : uint8_t {
  Byte = 1,
  Int32 = 4,
  Int64 = 8,
  Uint64 = 8,
  Double = 8,
  Decimal128 = 16,
};

enum struct kElement : uint8_t {
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

enum struct kBinaryElementSubtype : uint8_t {
  Generic = 0x00,
};

enum struct kBooleanElementValue : uint8_t {
  False = 0x00,
  True = 0x01,
};

} // namespace bson
} // namespace pot

#endif
