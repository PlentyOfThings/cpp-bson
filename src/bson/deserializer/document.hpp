#ifndef POT_BSON_DESERIALIZER_DOCUMENT_H_
#define POT_BSON_DESERIALIZER_DOCUMENT_H_

#include "../consts.hpp"
#include "../endian.hpp"
#include "./validation_options.hpp"
#include <cstdlib>

namespace pot {
namespace bson {
namespace deserializer {

class Array;

bool array_valid_(Array &arr);

#define __POT_BSON_VALID_SIZE_CHECK(buf_len, current, size) \
  if ((current + size) > buf_len) {                         \
    return false;                                           \
  }

#define __POT_BSON_VALID_TYPESIZE_CHECK(buf_len, current, type_size) \
  __POT_BSON_VALID_SIZE_CHECK(buf_len, current, static_cast<uint8_t>(type_size))

class Document {
public:
  Document(const uint8_t buf[], size_t len) :
      buffer_(buf), buffer_length_(len) {}

  template <size_t elm_name_buf_size = 50>
  bool valid(const ValidationOptions opts) {
    size_t current = 0;
    return valid<elm_name_buf_size>(opts, current);
  }

private:
  const uint8_t *buffer_;
  size_t buffer_length_;

  template <size_t elm_name_buf_size>
  bool valid(const ValidationOptions opts, size_t &current,
             bool array_doc = false) {
    size_t start = current;

    // Handle document size.
    __POT_BSON_VALID_TYPESIZE_CHECK(buffer_length_, current, TypeSize::Int32)
    int32_t doc_size =
        endian::buffer_to_primitive<int32_t, TypeSize::Int32>(buffer_, current);
    current += static_cast<uint8_t>(TypeSize::Int32);

    uint8_t element_type;
    char element_name[elm_name_buf_size];
    size_t element_index = 0;
    char element_index_str[kIntKeySize];
    while (true) {
      // Handle element type byte.
      __POT_BSON_VALID_TYPESIZE_CHECK(buffer_length_, current, TypeSize::Byte)
      element_type = buffer_[current];
      current += static_cast<uint8_t>(TypeSize::Byte);

      // A terminator means we've hit the end of the document.
      if (element_type == static_cast<uint8_t>(Element::Terminator)) {
        break;
      }

      // Handle element name, transferring it to the buffer we allocated before.
      size_t current_element_name_chr = 0;
      char name_chr;
      do {
        __POT_BSON_VALID_TYPESIZE_CHECK(buffer_length_, current, TypeSize::Byte)
        name_chr = buffer_[current];
        current += static_cast<uint8_t>(TypeSize::Byte);

        if (current_element_name_chr < elm_name_buf_size) {
          element_name[current_element_name_chr++] = name_chr;
        } else {
          return false;
        }
      } while (name_chr != '\0');

      // If we are validating an array document, and we want to valid the array
      // element names, then double check that the element name is the same as
      // the current index.
      if (array_doc && opts.valid_array_indices) {
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
          __POT_BSON_VALID_TYPESIZE_CHECK(buffer_length_, current,
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
          __POT_BSON_VALID_TYPESIZE_CHECK(buffer_length_, current,
                                          TypeSize::Int32)
          int32_t str_len =
              endian::buffer_to_primitive<int32_t, TypeSize::Int32>(buffer_,
                                                                    current);
          current += static_cast<uint8_t>(TypeSize::Int32);

          __POT_BSON_VALID_SIZE_CHECK(buffer_length_, current, str_len);
          // Size includes null terminator.
          if (buffer_[current + str_len - 1] != '\0') {
            return false;
          }
          current += str_len;
          break;
        }
        case static_cast<uint8_t>(Element::Document): {
          // Handle nested document.
          if (!valid<elm_name_buf_size>(opts, current)) {
            return false;
          }
          break;
        }
        case static_cast<uint8_t>(Element::Array): {
          // Handle nested array.
          // Since arrays are just documents with numerical indices,
          // we can use the same logic to handle them.
          if (!valid<elm_name_buf_size>(opts, current, true)) {
            return false;
          }
          break;
        }
        case static_cast<uint8_t>(Element::Binary): {
          // Handle binary element.
          __POT_BSON_VALID_TYPESIZE_CHECK(buffer_length_, current,
                                          TypeSize::Int32);
          int32_t bin_len =
              endian::buffer_to_primitive<int32_t, TypeSize::Int32>(buffer_,
                                                                    current);
          current += static_cast<uint8_t>(TypeSize::Int32);

          // We only support generic binary types right now.
          __POT_BSON_VALID_TYPESIZE_CHECK(buffer_length_, current,
                                          TypeSize::Byte);
          if (buffer_[current] !=
              static_cast<uint8_t>(BinaryElementSubtype::Generic)) {
            return false;
          }
          current += static_cast<uint8_t>(TypeSize::Byte);

          __POT_BSON_VALID_SIZE_CHECK(buffer_length_, current, bin_len)
          current += bin_len;
          break;
        }
        case static_cast<uint8_t>(Element::Boolean): {
          // Handle boolean element.
          __POT_BSON_VALID_TYPESIZE_CHECK(buffer_length_, current,
                                          TypeSize::Byte);
          uint8_t bool_val = buffer_[current];
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
          __POT_BSON_VALID_TYPESIZE_CHECK(buffer_length_, current,
                                          TypeSize::Int32)
          current += static_cast<uint8_t>(TypeSize::Int32);
          break;
        }
        case static_cast<uint8_t>(Element::Int64): {
          // Handle int64 element.
          __POT_BSON_VALID_TYPESIZE_CHECK(buffer_length_, current,
                                          TypeSize::Int64)
          current += static_cast<uint8_t>(TypeSize::Int64);
          break;
        }
        default:
          return false;
      }

      element_index++;
    }

    if (opts.correct_doc_length) {
      return (current - start) == doc_size;
    } else {
      return true;
    }
  }
};

} // namespace deserializer
} // namespace bson
} // namespace pot

#endif
