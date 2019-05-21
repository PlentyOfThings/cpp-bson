#ifndef POT_BSON_DESERIALIZER_DOCUMENT_H_
#define POT_BSON_DESERIALIZER_DOCUMENT_H_

#include "../consts.hpp"
#include "../endian.hpp"
#include "./array_element.hpp"
#include "./document_element.hpp"
#include <cstdlib>
#include <cstring>
#include <limits>

namespace pot {
namespace bson {
namespace deserializer {

class Array;
template <class Element> class ArrayIter;
template <class Element> class DocumentIter;

#define __POT_BSON_VALID_SIZE_CHECK(buf_len, current, size) \
  if ((current + size) > buf_len) {                         \
    return false;                                           \
  }

#define __POT_BSON_VALID_TYPESIZE_CHECK(buf_len, current, type_size) \
  __POT_BSON_VALID_SIZE_CHECK(buf_len, current, static_cast<uint8_t>(type_size))

struct GetElementResult {
  bool found;
  DocumentElement element;
};

class Document {
  typedef DocumentIter<DocumentElement> iterator;
  typedef std::ptrdiff_t difference_type;
  typedef size_t size_type;
  typedef DocumentElement value_type;
  typedef DocumentElement *pointer;
  typedef DocumentElement &reference;
  template <class Element> friend class ArrayIter;
  template <class Element> friend class DocumentIter;

  friend class ArrayElement;
  friend class DocumentElement;

public:
  Document() {}
  Document(const uint8_t buf[], const size_t len) :
      buffer_(buf), offset_(0), buffer_length_(len) {}

  template <size_t elm_name_buf_size = 50> bool valid() const {
    size_t current = 0;
    return valid<elm_name_buf_size>(current);
  }

  // Implemented in document_iter.hpp
  iterator begin() const;
  iterator end() const;
  bool getElByName(const char name[], DocumentElement &out) const;

protected:
  const uint8_t *buffer_;
  size_t offset_;
  size_t buffer_length_;

  Document(const uint8_t buf[], const size_t len, const size_t offset) :
      buffer_(buf), offset_(offset), buffer_length_(len) {}

  int32_t len() const {
    return endian::buffer_to_primitive<int32_t, TypeSize::Int32>(this->buffer_,
                                                                 this->offset_);
  }

  template <size_t elm_name_buf_size>
  bool valid(size_t &current, const bool array_doc = false) const {
    size_t start = current;

    // Handle document size.
    __POT_BSON_VALID_TYPESIZE_CHECK(this->buffer_length_, current,
                                    TypeSize::Int32)
    int32_t doc_size = endian::buffer_to_primitive<int32_t, TypeSize::Int32>(
        this->buffer_, current);
    current += static_cast<uint8_t>(TypeSize::Int32);

    uint8_t element_type;
    char element_name[elm_name_buf_size];
    size_t element_index = 0;
    char element_index_str[kIntKeySize];
    while (true) {
      // Handle element type byte.
      __POT_BSON_VALID_TYPESIZE_CHECK(this->buffer_length_, current,
                                      TypeSize::Byte)
      element_type = this->buffer_[current];
      current += static_cast<uint8_t>(TypeSize::Byte);

      // A terminator means we've hit the end of the document.
      if (element_type == static_cast<uint8_t>(Element::Terminator)) {
        break;
      }

      // Handle element name, transferring it to the buffer we allocated before.
      size_t current_element_name_chr = 0;
      char name_chr;
      do {
        __POT_BSON_VALID_TYPESIZE_CHECK(this->buffer_length_, current,
                                        TypeSize::Byte)
        name_chr = this->buffer_[current];
        current += static_cast<uint8_t>(TypeSize::Byte);

        if (current_element_name_chr < elm_name_buf_size) {
          element_name[current_element_name_chr++] = name_chr;
        } else {
          return false;
        }
      } while (name_chr != '\0');

      // If we are validating an array document then double check that the
      // element name is the same as the current index.
      if (array_doc) {
        convert_int_key_to_str(element_index, element_index_str);
        if (strncmp(element_name, element_index_str,
                    kIntKeySize < elm_name_buf_size ? kIntKeySize
                                                    : elm_name_buf_size) != 0) {
          return false;
        }
      }

      // Handle each element type.
      switch (element_type) {
        case static_cast<uint8_t>(Element::Double): {
          // Handle double element.
          __POT_BSON_VALID_TYPESIZE_CHECK(this->buffer_length_, current,
                                          TypeSize::Double)
          current += static_cast<uint8_t>(TypeSize::Double);
          break;
        }
        case static_cast<uint8_t>(Element::String): {
          // Handle string element.
          // BSON spec allows strings to contain null terminators,
          // so we _only_ use the size for validation.
          // We also have to account for the final null terminator
          // (although it doesn't help us at all).
          __POT_BSON_VALID_TYPESIZE_CHECK(this->buffer_length_, current,
                                          TypeSize::Int32)
          int32_t str_len =
              endian::buffer_to_primitive<int32_t, TypeSize::Int32>(
                  this->buffer_, current);
          current += static_cast<uint8_t>(TypeSize::Int32);

          __POT_BSON_VALID_SIZE_CHECK(this->buffer_length_, current, str_len);
          // Size includes null terminator.
          if (buffer_[current + str_len - 1] != '\0') {
            return false;
          }
          current += str_len;
          break;
        }
        case static_cast<uint8_t>(Element::Document): {
          // Handle nested document.
          if (!valid<elm_name_buf_size>(current)) {
            return false;
          }
          break;
        }
        case static_cast<uint8_t>(Element::Array): {
          // Handle nested array.
          // Since arrays are just documents with numerical indices,
          // we can use the same logic to handle them.
          if (!valid<elm_name_buf_size>(current, true)) {
            return false;
          }
          break;
        }
        case static_cast<uint8_t>(Element::Binary): {
          // Handle binary element.
          __POT_BSON_VALID_TYPESIZE_CHECK(this->buffer_length_, current,
                                          TypeSize::Int32);
          int32_t bin_len =
              endian::buffer_to_primitive<int32_t, TypeSize::Int32>(
                  this->buffer_, current);
          current += static_cast<uint8_t>(TypeSize::Int32);

          // We only support generic binary types right now.
          __POT_BSON_VALID_TYPESIZE_CHECK(this->buffer_length_, current,
                                          TypeSize::Byte);
          if (this->buffer_[current] !=
              static_cast<uint8_t>(BinaryElementSubtype::Generic)) {
            return false;
          }
          current += static_cast<uint8_t>(TypeSize::Byte);

          __POT_BSON_VALID_SIZE_CHECK(this->buffer_length_, current, bin_len)
          current += bin_len;
          break;
        }
        case static_cast<uint8_t>(Element::Boolean): {
          // Handle boolean element.
          __POT_BSON_VALID_TYPESIZE_CHECK(this->buffer_length_, current,
                                          TypeSize::Byte);
          uint8_t bool_val = this->buffer_[current];
          current += static_cast<uint8_t>(TypeSize::Byte);

          if (bool_val != static_cast<uint8_t>(BooleanElementValue::True) &&
              bool_val != static_cast<uint8_t>(BooleanElementValue::False)) {
            return false;
          }
          break;
        }
        case static_cast<uint8_t>(Element::Null):
          // Null elements have no content to validate.
          break;
        case static_cast<uint8_t>(Element::Int32): {
          // Handle int32 element.
          __POT_BSON_VALID_TYPESIZE_CHECK(this->buffer_length_, current,
                                          TypeSize::Int32)
          current += static_cast<uint8_t>(TypeSize::Int32);
          break;
        }
        case static_cast<uint8_t>(Element::Int64): {
          // Handle int64 element.
          __POT_BSON_VALID_TYPESIZE_CHECK(this->buffer_length_, current,
                                          TypeSize::Int64)
          current += static_cast<uint8_t>(TypeSize::Int64);
          break;
        }
        default:
          return false;
      }

      element_index++;
    }

    return (current - start) == doc_size;
  }
};

Document DocumentElement::getDoc() const {
  return { this->buffer_, this->buffer_length_,
           __POT_BSON_DOCUMENT_ELEMENT_DATA_OFFSET };
}

} // namespace deserializer
} // namespace bson
} // namespace pot

#endif
