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

  Array &append(double value) {
    char key[kKeySize];
    handleIndex(key);
    doc_.append(key, value);

    return *this;
  }

  Array &append(const char str[]) {
    char key[kKeySize];
    handleIndex(key);
    doc_.append(key, str);

    return *this;
  }

  Array &append(void (*builder)(Document &)) {
    char key[kKeySize];
    handleIndex(key);
    doc_.append(key, builder);

    return *this;
  }

  Array &append(void (*builder)(Array &)) {
    char key[kKeySize];
    handleIndex(key);
    doc_.append(key, builder);

    return *this;
  }

  Array &append(uint8_t buf[], size_t len) {
    char key[kKeySize];
    handleIndex(key);
    doc_.append(key, buf, len);

    return *this;
  }

  Array &append(bool value) {
    char key[kKeySize];
    handleIndex(key);
    doc_.append(key, value);

    return *this;
  }

  Array &append() {
    char key[kKeySize];
    handleIndex(key);
    doc_.append(key);

    return *this;
  }

  Array &append(int32_t value) {
    char key[kKeySize];
    handleIndex(key);
    doc_.append(key, value);

    return *this;
  }

  Array &append(int64_t value) {
    char key[kKeySize];
    handleIndex(key);
    doc_.append(key, value);

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
