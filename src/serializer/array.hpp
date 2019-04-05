#ifndef POT_BSON_SERIALIZER_ARRAY_H_
#define POT_BSON_SERIALIZER_ARRAY_H_

#include "./document.hpp"
#include "./result.hpp"
#include "stdlib.h"

namespace pot {
namespace bson {
namespace serializer {

class Array {
public:
  Array(uint8_t *buf, uint32_t len) : doc_(buf, len) {}
  Array(Document *parent) : doc_(parent) {}

  ~Array() {
    end();
  }

  kResult append(double value);
  kResult append(const char str[]);
  kResult append(uint8_t buf[]);
  kResult append(bool value);
  kResult append();
  kResult append(uint32_t value);
  kResult append(uint64_t value);

  kResult end();

private:
  Document doc_;
  uint32_t index_ = 0;
};

} // namespace serializer
} // namespace bson
} // namespace pot

#endif
