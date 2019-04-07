#ifndef POT_BSON_SERIALIZER_ARRAY_H_
#define POT_BSON_SERIALIZER_ARRAY_H_

#include "../consts.hpp"
#include "./document.hpp"
#include "./result.hpp"
#include <stdio.h>
#include <stdlib.h>

namespace pot {
namespace bson {
namespace serializer {

class Array : private Document {
public:
  Array(const char key[], Document *parent) :
      Document(key, parent, Element::Array) {}
  Array(Array &parent) : Document(parent, Element::Array) {}

  Array &appendDouble(double value) {
    char key[kArrayKeySize];
    handleIndex(key);
    Document::appendDouble(key, value);

    return *this;
  }

  Array &appendStr(const char str[]) {
    char key[kArrayKeySize];
    handleIndex(key);
    Document::appendStr(key, str);

    return *this;
  }

  Array &appendBinary(uint8_t buf[], size_t len) {
    char key[kArrayKeySize];
    handleIndex(key);
    Document::appendBinary(key, buf, len);

    return *this;
  }

  Array &appendBool(bool value) {
    char key[kArrayKeySize];
    handleIndex(key);
    Document::appendBool(key, value);

    return *this;
  }

  Array &appendNull() {
    char key[kArrayKeySize];
    handleIndex(key);
    Document::appendNull(key);

    return *this;
  }

  Array &appendInt32(int32_t value) {
    char key[kArrayKeySize];
    handleIndex(key);
    Document::appendInt32(key, value);

    return *this;
  }

  Array &appendInt64(int64_t value) {
    char key[kArrayKeySize];
    handleIndex(key);
    Document::appendInt64(key, value);

    return *this;
  }

  Document *getWorkingDoc() {
    return this;
  }

  int handleIndex(char key[]) {
    int ret = snprintf(key, kArrayKeySize, "%lu", index_);
    index_++;
    return ret;
  }

private:
  size_t index_ = 0;
};

Document *array_get_working_doc_(Array &arr) {
  return arr.getWorkingDoc();
}

int array_handle_index_(Array &arr, char key[]) {
  return arr.handleIndex(key);
}

} // namespace serializer
} // namespace bson
} // namespace pot

#endif
