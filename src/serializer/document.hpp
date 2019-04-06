#ifndef POT_BSON_SERIALIZER_DOCUMENT_H_
#define POT_BSON_SERIALIZER_DOCUMENT_H_

#include "../consts.hpp"
#include "../endian.hpp"
#include "./result.hpp"
#include <stdlib.h>
#include <string.h>

namespace pot {
namespace bson {
namespace serializer {

class Document {
public:
  static Result build(uint8_t buf[], size_t len, void (*builder)(Document &)) {
    Document doc(buf, len);
    doc.start();
    builder(doc);
    return doc.end();
  }

  Document(uint8_t buf[], size_t len) : buffer_(buf), buffer_length_(len) {}

  Document(Document &parent) :
      buffer_(parent.buffer_), buffer_length_(parent.buffer_length_),
      current_(parent.current_), start_(parent.start_) {}

  Document &append(const char key[], double value) {
    uint8_t buf[static_cast<size_t>(TypeSize::Double)];
    endian::primitive_to_buffer<double, TypeSize::Double>(buf, value);
    writeElement(Element::Double, key, buf, TypeSize::Double);

    return *this;
  }

  Document &append(const char key[], const char str[]) {
    writeByte(Element::String);
    writeStr(key);

    int32_t slen = strlen(str);
    writeInt32(slen + 1);
    writeStr(str);

    return *this;
  }

  Document &append(const char key[], void (*builder)(Document)) {
    Document child(*this);
    builder(child);
    child.end();
    syncWith(child);

    return *this;
  }

  Document &append(const char key[], uint8_t buf[], size_t len) {
    writeElement(Element::Binary, key, buf, len);

    return *this;
  }

  Document &append(const char key[], bool value) {
    writeByte(Element::Boolean);
    writeStr(key);

    if (value) {
      writeByte(static_cast<uint8_t>(BooleanElementValue::True));
    } else {
      writeByte(static_cast<uint8_t>(BooleanElementValue::False));
    }

    return *this;
  }

  Document &append(const char key[]) {
    writeByte(Element::Null);
    writeStr(key);

    return *this;
  }

  Document &append(const char key[], int32_t value) {
    writeByte(Element::Int32);
    writeStr(key);
    writeInt32(value);

    return *this;
  }

  Document &append(const char key[], int64_t value) {
    uint8_t buf[static_cast<size_t>(TypeSize::Int64)];
    endian::primitive_to_buffer<int64_t, TypeSize::Int64>(buf, value);
    writeElement(Element::Int64, key, buf, TypeSize::Int64);

    return *this;
  }

  Result end() {
    writeByte(0);

    // Store length of the document.
    int32_t len = current_ - start_;

    // Store the current buffer index.
    size_t tmp_current = current_;

    // Switch over to start.
    current_ = start_;

    // Write the length of the document.
    writeInt32(len);

    // Restore current buffer index.
    current_ = tmp_current;

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

private:
  uint8_t *buffer_;
  size_t buffer_length_;
  size_t current_ = 0;
  size_t start_ = 0;

  void start() {
    writeInt32(0);
  }

  void writeElement(Element type, const char key[], uint8_t buf[],
                    TypeSize size) {
    writeElement(type, key, buf, static_cast<size_t>(size));
  }

  void writeElement(Element type, const char key[], uint8_t value) {
    writeElement(type, key, &value, 1);
  }

  void writeElement(Element type, const char key[], uint8_t buf[], size_t len) {
    writeByte(type);
    writeStr(key);
    writeBuf(buf, len);
  }

  void writeStr(const char str[]) {
    size_t i = 0;
    char chr;
    while (true) {
      chr = str[i++];

      writeByte(chr);

      if (chr == '\0') {
        break;
      }
    }
  }

  void writeInt32(int32_t value) {
    uint8_t len_buf[static_cast<size_t>(TypeSize::Int32)];
    endian::primitive_to_buffer<int32_t, TypeSize::Int32>(len_buf, value);

    writeBuf(len_buf, TypeSize::Int32);
  }

  void writeBuf(uint8_t buf[], TypeSize size) {
    writeBuf(buf, static_cast<size_t>(size));
  }

  void writeBuf(uint8_t buf[], size_t len) {
    for (size_t i = 0; i < len; i++) {
      writeByte(buf[i]);
    }
  }

  void writeByte(Element type) {
    writeByte(static_cast<uint8_t>(type));
  }

  void writeByte(uint8_t byte) {
    if (current_ < buffer_length_) {
      buffer_[current_] = byte;
    }
    current_++;
  }
};

} // namespace serializer
} // namespace bson
} // namespace pot

#endif
