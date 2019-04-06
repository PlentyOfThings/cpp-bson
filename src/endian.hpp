#ifndef POT_BSON_ENDIAN_H_
#define POT_BSON_ENDIAN_H_

#include "./consts.hpp"
#include "stdlib.h"

namespace pot {
namespace bson {
namespace endian {

template <typename T, TypeSize Size> union ConvUnion {
  T value;
  uint8_t buf[static_cast<size_t>(Size)];
};

bool is_big_endian() {
  ConvUnion<uint32_t, TypeSize::Int32> convert = { 0x01020304 };

  return convert.buf[0] == 1;
}

template <typename T, TypeSize Size> void primitive_to_buffer(uint8_t buf[], T value) {
  ConvUnion<T, Size> convert = { value };
  uint8_t len = static_cast<uint8_t>(Size);

  if (is_big_endian()) {
    for (size_t i = 0; i < len; i++) {
      buf[i] = convert.buf[len - i];
    }
  } else {
    for (size_t i = 0; i < len; i++) {
      buf[i] = convert.buf[i];
    }
  }
}

template <typename T, TypeSize Size> T buffer_to_primitive(uint8_t buf[]) {
  ConvUnion<T, Size> convert;
  uint8_t len = static_cast<uint8_t>(Size);

  if (is_big_endian()) {
    for (size_t i = 0; i < len; i++) {
      convert.buf[len - i] = buf[i];
    }
  } else {
    for (size_t i = 0; i < len; i++) {
      convert.buf[i] = buf[i];
    }
  }

  return convert.value;
}

} // namespace endian
} // namespace bson
} // namespace pot

#endif
