#ifndef POT_BSON_SERIALIZER_DOCUMENT_H_
#define POT_BSON_SERIALIZER_DOCUMENT_H_

#include "../consts.hpp"
#include "../endian.hpp"
#include "./result.hpp"
#include <cstdlib>
#include <cstring>
#include <functional>

namespace pot {
namespace bson {
namespace serializer {

class Document;
class Array;

Document *array_get_working_doc_(Array &arr);
int array_handle_index_(Array &arr, char key[]);

class Document {
public:
  static Result build(uint8_t buf[], size_t len,
                      std::function<void(Document &)> builder) {
    Document doc(buf, len);
    builder(doc);

    return doc.end();
  }

  Document(uint8_t buf[], size_t len) : buffer_(buf), buffer_length_(len) {
    this->start();
  }

  Document(const char key[], Document *parent) :
      Document(key, parent, Element::Document) {}

  Document(Array &parent) : Document(parent, Element::Document) {}

  Document(const Document &) = delete;
  void operator=(const Document &) = delete;

  ~Document() {
    this->end();
  }

  Document &appendDouble(const char key[], double value) {
    uint8_t buf[static_cast<size_t>(TypeSize::Double)];
    endian::primitive_to_buffer<double, TypeSize::Double>(buf, value);
    this->writeElement(Element::Double, key, buf, TypeSize::Double);

    return *this;
  }

  Document &appendDouble(int32_t ikey, double value) {
    char skey[kIntKeySize];
    convert_int_key_to_str(ikey, skey);
    return this->appendDouble(skey, value);
  }

  Document &appendStr(const char key[], const char str[]) {
    this->writeByte(Element::String);
    this->writeStr(key);

    int32_t slen = strlen(str);
    this->writeInt32(slen + 1);
    this->writeStr(str);

    return *this;
  }

  Document &appendStr(int32_t ikey, const char str[]) {
    char skey[kIntKeySize];
    convert_int_key_to_str(ikey, skey);
    return this->appendStr(skey, str);
  }

  Document &appendDoc(const char key[],
                      std::function<void(Document &)> builder) {
    Document child(key, this);
    builder(child);

    return *this;
  }

  Document &appendDoc(int32_t ikey, std::function<void(Document &)> builder) {
    char skey[kIntKeySize];
    convert_int_key_to_str(ikey, skey);
    return this->appendDoc(skey, builder);
  }

  // Implemented in array.hpp
  Document &appendArr(const char key[], std::function<void(Array &)> builder);
  Document &appendArr(int32_t ikey, std::function<void(Array &)> builder);

  Document &appendBin(const char key[], const uint8_t buf[], int32_t len) {
    this->writeByte(Element::Binary);
    this->writeStr(key);

    this->writeInt32(len);
    this->writeByte(BinaryElementSubtype::Generic);
    this->writeBuf(buf, len);

    return *this;
  }

  Document &appendBin(int32_t ikey, const uint8_t buf[], int32_t len) {
    char skey[kIntKeySize];
    convert_int_key_to_str(ikey, skey);
    return this->appendBin(skey, buf, len);
  }

  Document &appendBool(const char key[], bool value) {
    this->writeByte(Element::Boolean);
    this->writeStr(key);

    if (value) {
      this->writeByte(static_cast<uint8_t>(BooleanElementValue::True));
    } else {
      this->writeByte(static_cast<uint8_t>(BooleanElementValue::False));
    }

    return *this;
  }

  Document &appendBool(int32_t ikey, bool value) {
    char skey[kIntKeySize];
    convert_int_key_to_str(ikey, skey);
    return this->appendBool(skey, value);
  }

  Document &appendNull(const char key[]) {
    this->writeByte(Element::Null);
    this->writeStr(key);

    return *this;
  }

  Document &appendNull(int32_t ikey) {
    char skey[kIntKeySize];
    convert_int_key_to_str(ikey, skey);
    return this->appendNull(skey);
  }

  Document &appendInt32(const char key[], int32_t value) {
    this->writeByte(Element::Int32);
    this->writeStr(key);
    this->writeInt32(value);

    return *this;
  }

  Document &appendInt32(int32_t ikey, int32_t value) {
    char skey[kIntKeySize];
    convert_int_key_to_str(ikey, skey);
    return this->appendInt32(skey, value);
  }

  Document &appendInt64(const char key[], int64_t value) {
    uint8_t buf[static_cast<size_t>(TypeSize::Int64)];
    endian::primitive_to_buffer<int64_t, TypeSize::Int64>(buf, value);
    this->writeElement(Element::Int64, key, buf, TypeSize::Int64);

    return *this;
  }

  Document &appendInt64(int32_t ikey, int64_t value) {
    char skey[kIntKeySize];
    convert_int_key_to_str(ikey, skey);
    return this->appendInt64(skey, value);
  }

  Result end() {
    int32_t len;
    if (!ended_) {
      this->writeByte(Element::Terminator);

      // Store length of the document.
      len = this->getLength();

      // Store the current buffer index.
      size_t tmp_current = current_;

      // Switch over to start.
      this->setCurrent(start_);

      // Write the length of the document.
      this->writeInt32(len);

      // Restore current buffer index.
      this->setCurrent(tmp_current);

      ended_ = true;
    } else {
      len = this->getLength();
    }

    Result res;
    res.len = len;
    if (current_ <= this->buffer_length_) {
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
    this->fromParent(parent, type);
    this->writeStr(key);
    this->start_ = parent->current_;
    this->start();
  }

  Document(Array &parent, Element type) {
    Document *docParent = array_get_working_doc_(parent);
    fromParent(docParent, type);
    char key[kIntKeySize];
    array_handle_index_(parent, key);
    this->writeStr(key);
    start_ = docParent->current_;
    this->start();
  }

  void fromParent(Document *parent, Element type) {
    this->buffer_ = parent->buffer_;
    this->buffer_length_ = parent->buffer_length_;
    this->current_ = parent->current_;
    this->parent_ = parent;
    this->writeByte(type);
  }

  void start() {
    this->writeInt32(0);
  }

  int32_t getLength() {
    return this->current_ - this->start_;
  }

  void writeElement(Element type, const char key[], uint8_t buf[],
                    TypeSize size) {
    this->writeElement(type, key, buf, static_cast<size_t>(size));
  }

  void writeElement(Element type, const char key[], uint8_t value) {
    this->writeElement(type, key, &value, 1);
  }

  void writeElement(Element type, const char key[], uint8_t buf[], size_t len) {
    this->writeByte(type);
    this->writeStr(key);
    this->writeBuf(buf, len);
  }

  void writeStr(const char str[]) {
    size_t i = 0;
    char chr;
    do {
      chr = str[i++];
      this->writeByte(chr);
    } while (chr != '\0');
  }

  void writeInt32(int32_t value) {
    uint8_t len_buf[static_cast<size_t>(TypeSize::Int32)];
    endian::primitive_to_buffer<int32_t, TypeSize::Int32>(len_buf, value);

    this->writeBuf(len_buf, TypeSize::Int32);
  }

  void writeBuf(uint8_t buf[], TypeSize size) {
    this->writeBuf(buf, static_cast<size_t>(size));
  }

  void writeBuf(const uint8_t buf[], size_t len) {
    for (size_t i = 0; i < len; i++) {
      this->writeByte(buf[i]);
    }
  }

  void writeByte(Element type) {
    this->writeByte(static_cast<uint8_t>(type));
  }

  void writeByte(BinaryElementSubtype bin_type) {
    this->writeByte(static_cast<uint8_t>(bin_type));
  }

  void writeByte(uint8_t byte) {
    if (current_ < buffer_length_) {
      buffer_[current_] = byte;
    }
    this->incCurrent();
  }

  void incCurrent() {
    this->current_++;
    if (this->parent_) {
      this->parent_->incCurrent();
    }
  }

  void setCurrent(size_t cur) {
    this->current_ = cur;
    if (this->parent_) {
      this->parent_->setCurrent(cur);
    }
  }
};

} // namespace serializer
} // namespace bson
} // namespace pot

#endif
