#ifndef POT_BSON_SERIALIZER_ARRAY_H_
#define POT_BSON_SERIALIZER_ARRAY_H_

#include "./document.hpp"
#include "./result.hpp"
#include <stdio.h>
#include <stdlib.h>

namespace pot {
namespace bson {
namespace serializer {

static constexpr size_t kKeySize = 12;

class Array {
public:
  static Array from(Document &current) {
    Array arr(current);
    return arr;
  }

  Array(Document &current) : doc_(current) {}

  Array &appendDouble(double value) {
    char key[kKeySize];
    handleIndex(key);
    doc_.appendDouble(key, value);

    return *this;
  }

  Array &appendStr(const char str[]) {
    char key[kKeySize];
    handleIndex(key);
    doc_.appendStr(key, str);

    return *this;
  }

  Array &appendDoc(void (*builder)(Document &)) {
    char key[kKeySize];
    handleIndex(key);
    doc_.appendDoc(key, builder);

    return *this;
  }

  Array &appendBinary(uint8_t buf[], size_t len) {
    char key[kKeySize];
    handleIndex(key);
    doc_.appendBinary(key, buf, len);

    return *this;
  }

  Array &appendBool(bool value) {
    char key[kKeySize];
    handleIndex(key);
    doc_.appendBool(key, value);

    return *this;
  }

  Array &appendNull() {
    char key[kKeySize];
    handleIndex(key);
    doc_.appendNull(key);

    return *this;
  }

  Array &appendInt32(int32_t value) {
    char key[kKeySize];
    handleIndex(key);
    doc_.appendInt32(key, value);

    return *this;
  }

  Array &appendInt64(int64_t value) {
    char key[kKeySize];
    handleIndex(key);
    doc_.appendInt64(key, value);

    return *this;
  }

private:
  Document doc_;
  size_t index_ = 0;

  int handleIndex(char key[]) {
    int ret = snprintf(key, kKeySize, "%lu", index_);
    index_++;
    return ret;
  }
};

} // namespace serializer
} // namespace bson
} // namespace pot

#endif
