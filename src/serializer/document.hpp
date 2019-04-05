#ifndef POT_BSON_SERIALIZER_DOCUMENT_H_
#define POT_BSON_SERIALIZER_DOCUMENT_H_

#include "../consts.hpp"
#include "./array.hpp"
#include "./result.hpp"
#include "stdlib.h"

namespace pot {
namespace bson {
namespace serializer {

class Document {
public:
  Document(uint8_t buf[], size_t len) : buffer_(buf), buffer_length_(len) {
    start();
  }
  Document(Document &parent) :
      buffer_(parent.buffer_), buffer_length_(parent.buffer_length_), current_(parent.current_),
      start_(parent.start_) {
    start();
  }

  Document &append(const char key[], double value);

  Document &append(const char key[], const char str[]);

  Document &append(const char key[], void (*builder)(Document)) {
    Document child(*this);
    builder(child);
    syncWith(child);

    return *this;
  }

  Document &append(const char key[], void (*builder)(Array)) {
    Array child(*this);
    builder(child);
    syncWith(child);

    return *this;
  }

  Document &append(const char key[], uint8_t buf[], size_t len);

  Document &append(const char key[], bool value);

  Document &append(const char key[]);

  Document &append(const char key[], uint32_t value);

  Document &append(const char key[], uint64_t value);

  Result end() {
    uint8_t endByte = 0x00;
    write(&endByte, 1);

    uint32_t len = current_ - start_;

    // Attempt to write length of the document into the start.
    if (start_ < buffer_length_) {
      buffer_[start_] = len;
    }

    Result res;
    res.len = len;
    if (current_ <= buffer_length_) {
      res.status = Status::Ok;
    } else {
      res.status = Status::BufferOverflow;
    }

    return res;
  }

  void syncWith(Document &child) {
    current_ = child.current_;
  }

  void syncWith(Array &child) {
    child.syncTo(*this);
  }

private:
  uint8_t *buffer_;
  size_t buffer_length_;
  uint32_t current_ = 0;
  uint32_t start_ = 0;
  bool ended_ = false;

  void start() {
    if (current_ < buffer_length_) {
      // The document is currently 0 bytes long.
      buffer_[current_] = 0x00;
    }
    current_++;
  }

  void write(Element type, const char key[], uint8_t buf[], size_t len) {
    if (current_ < buffer_length_) {
      buffer_[current_] = (uint8_t)type;
    }
    current_++;

    uint32_t key_char = 0;
    while (true) {
      char chr = key[key_char];

      if (chr == '\0') {
        break;
      }

      if (current_ < buffer_length_) {
        buffer_[current_] = chr;
      }
      current_++;
    }

    write(buf, len);
  }

  void write(uint8_t buf[], size_t len) {
    for (uint32_t i = 0; i < len; i++) {
      if (current_ < buffer_length_) {
        buffer_[current_] = buf[i];
      }
      current_++;
    }
  }
};

} // namespace serializer
} // namespace bson
} // namespace pot

#endif
