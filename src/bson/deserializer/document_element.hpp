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

  bool nameEquals(const char str[]) const {
    return is_string_equal(this->buffer_, str,
                           __POT_BSON_DOCUMENT_ELEMENT_NAME_OFFSET);
  }

  double getDouble() const {
    return endian::buffer_to_primitive<double, TypeSize::Double>(
        this->buffer_, __POT_BSON_DOCUMENT_ELEMENT_DATA_OFFSET);
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

  int getStr(char str[], const size_t len) const {
    return copy_string_to(this->buffer_, str,
                          __POT_BSON_DOCUMENT_ELEMENT_DATA_LEN_OFFSET, len);
  }

  bool strEquals(const char str[]) const {
    return is_string_equal(this->buffer_, str,
                           __POT_BSON_DOCUMENT_ELEMENT_DATA_LEN_OFFSET);
  }

  // Implemented in document.hpp
  Document getDoc() const;

  // Implemented in array.hpp
  Array getArr() const;

  int getBinary(uint8_t buf[], const size_t len) const {
    int32_t bin_len = getDataLen();
    size_t offset = __POT_BSON_DOCUMENT_ELEMENT_DATA_LEN_OFFSET +
                    static_cast<uint8_t>(TypeSize::Byte);
    size_t copy_len = bin_len < len ? bin_len : len;

    for (size_t i = 0; i < copy_len; i++) {
      buf[i] = this->buffer_[offset + i];
    }

    return copy_len;
  }

  bool getBool() const {
    uint8_t val = this->buffer_[__POT_BSON_DOCUMENT_ELEMENT_DATA_OFFSET];

    return val == static_cast<uint8_t>(BooleanElementValue::True);
  }

  int32_t getInt32() const {
    return endian::buffer_to_primitive<int32_t, TypeSize::Int32>(
        this->buffer_, __POT_BSON_DOCUMENT_ELEMENT_DATA_OFFSET);
  }

  int64_t getInt64() const {
    return endian::buffer_to_primitive<int64_t, TypeSize::Int64>(
        this->buffer_, __POT_BSON_DOCUMENT_ELEMENT_DATA_OFFSET);
  }

  int64_t getInt() const {
    if (type() == Element::Int32) {
      return getInt32();
    } else {
      return getInt64();
    }
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

  int32_t getDataLen() const {
    return endian::buffer_to_primitive<int32_t, TypeSize::Int32>(
        this->buffer_, __POT_BSON_DOCUMENT_ELEMENT_DATA_OFFSET);
  }
};

} // namespace deserializer
} // namespace bson
} // namespace pot

#endif
