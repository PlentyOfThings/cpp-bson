#ifndef POT_BSON_DESERIALIZER_ARRAY_ELEMENT_H_
#define POT_BSON_DESERIALIZER_ARRAY_ELEMENT_H_

#include "./document_element.hpp"
#include <cstdlib>

namespace pot {
namespace bson {
namespace deserializer {

class ArrayElement : public DocumentElement {
public:
  ArrayElement(const uint8_t buf[], const size_t start, const size_t len) :
      DocumentElement(buf, start, len), index_(0) {}

  ArrayElement(const uint8_t buf[], const size_t start, const size_t len,
               const size_t index) :
      DocumentElement(buf, start, len),
      index_(index) {}

  size_t getIndex() const {
    return this->index_;
  }

private:
  const size_t index_;
};

} // namespace deserializer
} // namespace bson
} // namespace pot

#endif
