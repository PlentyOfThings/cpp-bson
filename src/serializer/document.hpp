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

class Document;
class Array;

Document *array_get_working_doc_(Array &arr);
int array_handle_index_(Array &arr, char key[]);

class Document {
public:
  Document(uint8_t buf[], size_t len) : buffer_(buf), buffer_length_(len) {
    start();
  }

  Document(const char key[], Document *parent) :
      Document(key, parent, Element::Document) {}

  Document(Array &parent) : Document(parent, Element::Document) {}

  Document(const Document &) = delete;
  void operator=(const Document &) = delete;

  ~Document() {
    end();
  }

  Document &appendDouble(const char key[], double value) {
    uint8_t buf[static_cast<size_t>(TypeSize::Double)];
    endian::primitive_to_buffer<double, TypeSize::Double>(buf, value);
    writeElement(Element::Double, key, buf, TypeSize::Double);

    return *this;
  }

  Document &appendStr(const char key[], const char str[]) {
    writeByte(Element::String);
    writeStr(key);

    int32_t slen = strlen(str);
    writeInt32(slen + 1);
    writeStr(str);

    return *this;
  }

  Document &appendBinary(const char key[], uint8_t buf[], int32_t len) {
    writeByte(Element::Binary);
    writeStr(key);

    writeInt32(len);
    writeByte(BinaryElementSubtype::Generic);
    writeBuf(buf, len);

    return *this;
  }

  Document &appendBool(const char key[], bool value) {
    writeByte(Element::Boolean);
    writeStr(key);

    if (value) {
      writeByte(static_cast<uint8_t>(BooleanElementValue::True));
    } else {
      writeByte(static_cast<uint8_t>(BooleanElementValue::False));
    }

    return *this;
  }

  Document &appendNull(const char key[]) {
    writeByte(Element::Null);
    writeStr(key);

    return *this;
  }

  Document &appendInt32(const char key[], int32_t value) {
    writeByte(Element::Int32);
    writeStr(key);
    writeInt32(value);

    return *this;
  }

  Document &appendInt64(const char key[], int64_t value) {
    uint8_t buf[static_cast<size_t>(TypeSize::Int64)];
    endian::primitive_to_buffer<int64_t, TypeSize::Int64>(buf, value);
    writeElement(Element::Int64, key, buf, TypeSize::Int64);

    return *this;
  }

  Result end() {
    int32_t len;
    if (!ended_) {
      writeByte(0);

      // Store length of the document.
      len = getLength();

      // Store the current buffer index.
      size_t tmp_current = current_;

      // Switch over to start.
      setCurrent(start_);

      // Write the length of the document.
      writeInt32(len);

      // Restore current buffer index.
      setCurrent(tmp_current);

      ended_ = true;
    } else {
      len = getLength();
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

protected:
  uint8_t *buffer_;
  size_t buffer_length_;
  size_t current_ = 0;
  size_t start_ = 0;
  Document *parent_ = nullptr;
  bool ended_ = false;

  Document(const char key[], Document *parent, Element type) {
    fromParent(parent, type);
    writeStr(key);
    start_ = parent->current_;
    start();
  }

  Document(Array &parent, Element type) {
    Document *docParent = array_get_working_doc_(parent);
    fromParent(docParent, type);
    char key[kArrayKeySize];
    array_handle_index_(parent, key);
    writeStr(key);
    start_ = docParent->current_;
    start();
  }

  void fromParent(Document *parent, Element type) {
    buffer_ = parent->buffer_;
    buffer_length_ = parent->buffer_length_;
    current_ = parent->current_;
    parent_ = parent;
    writeByte(type);
  }

  void start() {
    writeInt32(0);
  }

  int32_t getLength() {
    return current_ - start_;
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

  void writeByte(BinaryElementSubtype binType) {
    writeByte(static_cast<uint8_t>(binType));
  }

  void writeByte(uint8_t byte) {
    if (current_ < buffer_length_) {
      buffer_[current_] = byte;
    }
    incCurrent();
  }

  void incCurrent() {
    current_++;
    if (parent_) {
      parent_->incCurrent();
    }
  }

  void setCurrent(size_t cur) {
    current_ = cur;
    if (parent_) {
      parent_->setCurrent(cur);
    }
  }
};

} // namespace serializer
} // namespace bson
} // namespace pot

#endif
