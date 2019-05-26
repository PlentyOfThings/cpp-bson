#ifndef POT_BSON_DESERIALIZER_DOCUMENT_ELEMENT_H_
#define POT_BSON_DESERIALIZER_DOCUMENT_ELEMENT_H_

#include "../consts.hpp"
#include "../endian.hpp"
#include <cstdlib>

namespace pot {
namespace bson {
namespace deserializer {

int copy_string_to(const uint8_t from[], char to[], const size_t start,
                   const size_t len) {
  size_t i = 0;
  char chr;
  for (; i < len - 1; i++) {
    chr = from[start + i];
    to[i] = chr;

    if (chr == '\0') {
      break;
    }
  }

  // Make sure the last element is a null terminator in case
  // the destination buffer is too short.
  // This gurantees that the string is valid.
  to[i] = '\0';

  return i + 1;
}

bool is_string_equal(const uint8_t buf[], const char cmp[],
                     const size_t start) {
  size_t i = 0;
  char chr;
  do {
    chr = buf[start + i];

    if (chr != cmp[i]) {
      return false;
    }

    i++;
  } while (chr != '\0');

  return true;
}

class Document;
class Array;

namespace data_type {

struct Str {
  const char *str;
  int64_t len;
};

struct Doc;

struct Arr;

struct Bin {
  const uint8_t *bin;
  int64_t len;
};

} // namespace data_type

#define __POT_BSON_DOCUMENT_ELEMENT_NAME_OFFSET \
  this->start_ + static_cast<uint8_t>(TypeSize::Byte)

#define __POT_BSON_DOCUMENT_ELEMENT_DATA_OFFSET \
  __POT_BSON_DOCUMENT_ELEMENT_NAME_OFFSET + this->nameSize()

#define __POT_BSON_DOCUMENT_ELEMENT_DATA_LEN_OFFSET \
  __POT_BSON_DOCUMENT_ELEMENT_DATA_OFFSET +         \
      static_cast<uint8_t>(TypeSize::Int32)

class DocumentElement {
public:
  DocumentElement() {}

  DocumentElement(const uint8_t buf[], const size_t start, const size_t len) :
      buffer_(buf), start_(start), buffer_length_(len) {}

  Element type() const {
    return static_cast<Element>(this->buffer_[this->start_]);
  }

  bool isNumber() const {
    auto type = this->type();
    return type == Element::Int32 || type == Element::Int64 ||
           type == Element::Double;
  }

  bool isInt() const {
    auto type = this->type();
    return type == Element::Int32 || type == Element::Int64;
  }

  int getName(char str[], const size_t len) const {
    return copy_string_to(this->buffer_, str,
                          __POT_BSON_DOCUMENT_ELEMENT_NAME_OFFSET, len);
  }

  const char *getNameRef() const {
    return reinterpret_cast<const char *>(
        &this->buffer_[__POT_BSON_DOCUMENT_ELEMENT_NAME_OFFSET]);
  }

  bool nameEquals(const char str[]) const {
    return is_string_equal(this->buffer_, str,
                           __POT_BSON_DOCUMENT_ELEMENT_NAME_OFFSET);
  }

  double getDouble() const {
    return endian::buffer_to_primitive<double, TypeSize::Double>(
        this->buffer_, __POT_BSON_DOCUMENT_ELEMENT_DATA_OFFSET);
  }

  bool tryGetDouble(double &out) const {
    if (this->type() != Element::Double) {
      return false;
    }

    out = this->getDouble();
    return true;
  }

  double getNumber() const {
    auto type = this->type();
    if (type == Element::Int32) {
      return getInt32();
    } else if (type == Element::Int64) {
      return getInt64();
    } else {
      return getDouble();
    }
  }

  bool tryGetNumber(double &out) const {
    if (!this->isNumber()) {
      return false;
    }

    out = this->getNumber();
    return true;
  }

  int getStr(char str[], const size_t len) const {
    return copy_string_to(this->buffer_, str,
                          __POT_BSON_DOCUMENT_ELEMENT_DATA_LEN_OFFSET, len);
  }

  /**
   * Returns the pointer reference to the contained string.
   * Use
   *     DocumentElement::getStrLen()
   * to get the length of the string.
   * The pointer is only valid for as long as the buffer data is.
   */
  const char *getStrRef() const {
    return reinterpret_cast<const char *>(
        &this->buffer_[__POT_BSON_DOCUMENT_ELEMENT_DATA_LEN_OFFSET]);
  }

  bool strEquals(const char str[]) const {
    return is_string_equal(this->buffer_, str,
                           __POT_BSON_DOCUMENT_ELEMENT_DATA_LEN_OFFSET);
  }

  int64_t getStrLen() const {
    // Exclude the null-terminator
    return this->getDataLen() - 1;
  }

  bool tryGetStr(data_type::Str &out) const {
    if (this->type() != Element::String) {
      return false;
    }

    out = { .str = this->getStrRef(), .len = this->getStrLen() };
    return true;
  }

  // Implemented in document.hpp
  Document getDoc() const;
  bool tryGetDoc(data_type::Doc &out) const;

  int64_t getDocLen() const {
    return this->getDataLen();
  }

  // Implemented in array.hpp
  Array getArr() const;
  bool tryGetArr(data_type::Arr &out) const;

  int64_t getArrLen() const {
    return this->getDataLen();
  }

  int getBin(uint8_t buf[], const size_t len) const {
    int32_t bin_len = getDataLen();
    size_t offset = __POT_BSON_DOCUMENT_ELEMENT_DATA_LEN_OFFSET +
                    static_cast<uint8_t>(TypeSize::Byte);
    size_t copy_len = bin_len < len ? bin_len : len;

    for (size_t i = 0; i < copy_len; i++) {
      buf[i] = this->buffer_[offset + i];
    }

    return copy_len;
  }

  /**
   * Returns the pointer reference to the contained binary array.
   * Use
   *     DocumentElement::getBinaryLen()
   * to get the length of the array.
   * The pointer is only valid for as long as the buffer data is.
   */
  const uint8_t *getBinRef() const {
    return &this->buffer_[__POT_BSON_DOCUMENT_ELEMENT_DATA_LEN_OFFSET +
                          static_cast<uint8_t>(TypeSize::Byte)];
  }

  int64_t getBinLen() const {
    return this->getDataLen();
  }

  bool tryGetBin(data_type::Bin &out) const {
    if (this->type() != Element::Binary) {
      return false;
    }

    out = { .bin = this->getBinRef(), .len = this->getBinLen() };
    return true;
  }

  bool getBool() const {
    uint8_t val = this->buffer_[__POT_BSON_DOCUMENT_ELEMENT_DATA_OFFSET];

    return val == static_cast<uint8_t>(BooleanElementValue::True);
  }

  bool tryGetBool(bool &out) const {
    if (this->type() != Element::Boolean) {
      return false;
    }

    out = this->getBool();
    return true;
  }

  int32_t getInt32() const {
    return endian::buffer_to_primitive<int32_t, TypeSize::Int32>(
        this->buffer_, __POT_BSON_DOCUMENT_ELEMENT_DATA_OFFSET);
  }

  bool tryGetInt32(int32_t &out) const {
    if (this->type() != Element::Int32) {
      return false;
    }

    out = this->getInt32();
    return true;
  }

  int64_t getInt64() const {
    return endian::buffer_to_primitive<int64_t, TypeSize::Int64>(
        this->buffer_, __POT_BSON_DOCUMENT_ELEMENT_DATA_OFFSET);
  }

  bool tryGetInt64(int64_t &out) const {
    if (this->type() != Element::Int64) {
      return false;
    }

    out = this->getInt64();
    return true;
  }

  int64_t getInt() const {
    if (type() == Element::Int32) {
      return getInt32();
    } else {
      return getInt64();
    }
  }

  bool tryGetInt(int64_t &out) const {
    if (!this->isInt()) {
      return false;
    }

    out = this->getInt();
    return true;
  }

  size_t nameSize() const {
    if (this->name_size_ > 0) {
      return this->name_size_;
    }

    size_t sz = 0;

    char chr;
    do {
      chr = this->buffer_[this->start_ + sz + 1];
      sz++;
    } while (chr != '\0');

    this->name_size_ = sz;
    return sz;
  }

  size_t dataSize() const {
    Element tp = this->type();
    switch (tp) {
      case Element::Double: {
        return static_cast<uint8_t>(TypeSize::Double);
      }
      case Element::String:
      case Element::Document:
      case Element::Array:
      case Element::Binary: {
        // Get the size of the data.
        int32_t sz = this->getDataLen();

        // Documents and arrays have the length bytes accounted for in the
        // main data length value.
        if (tp != Element::Document && tp != Element::Array) {
          sz += static_cast<uint8_t>(TypeSize::Int32);
        }

        // Binary types also have the subtype byte.
        if (tp == Element::Binary) {
          sz += static_cast<uint8_t>(TypeSize::Byte);
        }

        return sz;
      }
      case Element::Boolean: {
        return static_cast<uint8_t>(TypeSize::Byte);
      }
      case Element::Null: {
        return 0;
      }
      case Element::Int32: {
        return static_cast<uint8_t>(TypeSize::Int32);
      }
      case Element::Int64: {
        return static_cast<uint8_t>(TypeSize::Int64);
      }
      case Element::Terminator: {
        return 0;
      }
    }
  }

private:
  const uint8_t *buffer_;
  size_t start_;
  size_t buffer_length_;
  mutable size_t name_size_ = 0;

  /**
   * Returns the length of the element's value in bytes.
   * Supports:
   * - String
   * - Document
   * - Array
   * - Binary
   * For strings, this includes the null terminator.
   */
  int32_t getDataLen() const {
    return endian::buffer_to_primitive<int32_t, TypeSize::Int32>(
        this->buffer_, __POT_BSON_DOCUMENT_ELEMENT_DATA_OFFSET);
  }
};

} // namespace deserializer
} // namespace bson
} // namespace pot

#endif
