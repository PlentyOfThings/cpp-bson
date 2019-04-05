#ifndef POT_BSON_SERIALIZER_DOCUMENT_H_
#define POT_BSON_SERIALIZER_DOCUMENT_H_

#include "./result.hpp"
#include "stdlib.h"

namespace pot {
namespace bson {
namespace serializer {

class Document {
public:
  Document(uint8_t buf[], uint32_t len) : buffer_(buf), buffer_length_(len) {}
  Document(Document *parent) :
      buffer_(parent->buffer_), buffer_length_(parent->buffer_length_), current_(parent->current_),
      parent_(parent) {}

  ~Document() {
    end();
  }

  kResult append(const char key[], double value);
  kResult append(const char key[], const char str[]);
  kResult append(const char key[], uint8_t buf[]);
  kResult append(const char key[], bool value);
  kResult append(const char key[]);
  kResult append(const char key[], uint32_t value);
  kResult append(const char key[], uint64_t value);

  kResult end();

private:
  uint8_t *buffer_;
  uint32_t buffer_length_;
  uint32_t current_ = 0;
  Document *parent_ = nullptr;
  bool ended_ = false;

  bool start();
};

} // namespace serializer
} // namespace bson
} // namespace pot

#endif
