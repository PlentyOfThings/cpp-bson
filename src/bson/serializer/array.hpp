#ifndef POT_BSON_SERIALIZER_ARRAY_H_
#define POT_BSON_SERIALIZER_ARRAY_H_

#include "../consts.hpp"
#include "./document.hpp"
#include "./result.hpp"
#include <cstdio>
#include <cstdlib>
#include <functional>

namespace pot {
namespace bson {
namespace serializer {

class Array : private Document {
public:
  Array(const char key[], Document *parent) :
      Document(key, parent, Element::Array) {}
  Array(Array &parent) : Document(parent, Element::Array) {}

  Array(const Array &) = delete;
  void operator=(const Array &) = delete;

  Array &appendDouble(double value) {
    Document::appendDouble(index_++, value);

    return *this;
  }

  Array &appendStr(const char str[]) {
    Document::appendStr(index_++, str);

    return *this;
  }

  Array &appendDoc(std::function<void(Document &)> builder) {
    Document::appendDoc(index_++, builder);

    return *this;
  }

  Array &appendArr(std::function<void(Array &)> builder) {
    Document::appendArr(index_++, builder);

    return *this;
  }

  Array &appendBinary(const uint8_t buf[], size_t len) {
    Document::appendBinary(index_++, buf, len);

    return *this;
  }

  Array &appendBool(bool value) {
    Document::appendBool(index_++, value);

    return *this;
  }

  Array &appendNull() {
    Document::appendNull(index_++);

    return *this;
  }

  Array &appendInt32(int32_t value) {
    Document::appendInt32(index_++, value);

    return *this;
  }

  Array &appendInt64(int64_t value) {
    Document::appendInt64(index_++, value);

    return *this;
  }

  Document *getWorkingDoc() {
    return this;
  }

  int handleIndex(char key[]) {
    int ret = convert_int_key_to_str(index_, key);
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

// Implementations for members in document.hpp
Document &Document::appendArr(const char key[],
                              std::function<void(Array &)> builder) {
  Array child(key, this);
  builder(child);

  return *this;
}

Document &Document::appendArr(int32_t ikey,
                              std::function<void(Array &)> builder) {
  char skey[kIntKeySize];
  convert_int_key_to_str(ikey, skey);
  return appendArr(skey, builder);
}

} // namespace serializer
} // namespace bson
} // namespace pot

#endif
