#ifndef POT_BSON_DESERIALIZER_ARRAY_H_
#define POT_BSON_DESERIALIZER_ARRAY_H_

#include "./array_element.hpp"
#include "./document.hpp"
#include "./document_element.hpp"
#include <cstdlib>

namespace pot {
namespace bson {
namespace deserializer {

class Array : public Document {
  typedef ArrayIter<ArrayElement> iterator;
  typedef std::ptrdiff_t difference_type;
  typedef size_t size_type;
  typedef ArrayElement value_type;
  typedef ArrayElement *pointer;
  typedef ArrayElement &reference;
  template <class Element> friend class ArrayIter;
  template <class Element> friend class DocumentIter;

  friend class ArrayElement;
  friend class DocumentElement;

public:
  Array(const uint8_t buf[], const size_t len) : Document::Document(buf, len) {}

  // Implemented in array_iter.hpp
  iterator begin() const;
  iterator end() const;
  std::shared_ptr<ArrayElement> getElByName(const char index[]) const;
  std::shared_ptr<ArrayElement> getElByIndex(const size_t index) const;
  bool containsDouble(const double value, const double epsilon) const;
  bool containsStr(const char str[]) const;
  bool containsBool(const bool value) const;
  bool containsNull() const;
  bool containsInt32(const int32_t value) const;
  bool containsInt64(const int64_t value) const;

protected:
  Array(const uint8_t buf[], const size_t len, const size_t offset) :
      Document::Document(buf, len, offset) {}
};

Array DocumentElement::getArr() const {
  return { this->buffer_, this->buffer_length_,
           __POT_BSON_DOCUMENT_ELEMENT_DATA_OFFSET };
}

} // namespace deserializer
} // namespace bson
} // namespace pot

#endif
