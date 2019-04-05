#ifndef POT_BSON_SERIALIZER_ARRAY_H_
#define POT_BSON_SERIALIZER_ARRAY_H_

#include "./document.hpp"
#include "./result.hpp"
#include "stdio.h"
#include "stdlib.h"

namespace pot {
namespace bson {
namespace serializer {

class Array {
public:
  Array(Document &parent) : doc_(parent) {}

  Array &append(double value) {
    char key[12];
    handleIndex(key);
    doc_.append(key, value);

    return *this;
  }

  Array &append(const char str[]) {
    char key[12];
    handleIndex(key);
    doc_.append(key, str);

    return *this;
  }

  Array &append(void (*builder)(Document)) {
    char key[12];
    handleIndex(key);
    doc_.append(key, builder);

    return *this;
  }

  Array &append(void (*builder)(Array)) {
    char key[12];
    handleIndex(key);
    doc_.append(key, builder);

    return *this;
  }

  Array &append(uint8_t buf[], size_t len) {
    char key[12];
    handleIndex(key);
    doc_.append(key, buf, len);

    return *this;
  }

  Array &append(bool value) {
    char key[12];
    handleIndex(key);
    doc_.append(key, value);

    return *this;
  }

  Array &append() {
    char key[12];
    handleIndex(key);
    doc_.append(key);

    return *this;
  }

  Array &append(uint32_t value) {
    char key[12];
    handleIndex(key);
    doc_.append(key, value);

    return *this;
  }

  Array &append(uint64_t value) {
    char key[12];
    handleIndex(key);
    doc_.append(key, value);

    return *this;
  }

  Result end() {
    return doc_.end();
  }

  void syncWith(Document &child) {
    doc_.syncWith(child);
  }

  void syncWith(Array &child) {
    doc_.syncWith(child);
  }

  void syncTo(Document &parent) {
    parent.syncWith(doc_);
  }

private:
  Document doc_;
  size_t index_ = 0;

  int handleIndex(char key[]) {
    int ret = snprintf(key, sizeof(key), "%Lu", index_);
    index_++;
    return ret;
  }
};

} // namespace serializer
} // namespace bson
} // namespace pot

#endif
