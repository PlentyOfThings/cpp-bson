#ifndef POT_BSON_DESERIALIZER_ARRAY_ITER_H_
#define POT_BSON_DESERIALIZER_ARRAY_ITER_H_

#include "../consts.hpp"
#include "./array.hpp"
#include "./document_iter.hpp"
#include <cstdlib>

namespace pot {
namespace bson {
namespace deserializer {

template <class Element> class ArrayIter : public DocumentIter<Element> {
public:
  ArrayIter(const Document &doc) : DocumentIter<Element>(doc) {}

  ArrayIter(const Document &doc, size_t current) :
      DocumentIter<Element>(doc, current) {}

  virtual Element operator*() const {
    return { DocumentIter<Element>::doc_.buffer_,
             DocumentIter<Element>::current_,
             DocumentIter<Element>::doc_.buffer_length_, index_ };
  }

  virtual ArrayIter &operator++() {
    index_++;
    DocumentIter<Element>::operator++();
    return *this;
  }

private:
  size_t index_ = 0;
};

Array::iterator Array::begin() const {
  return Array::iterator(*this);
}

Array::iterator Array::end() const {
  return Array::iterator(*this,
                         this->Document::offset_ + this->Document::len() - 1);
}

} // namespace deserializer
} // namespace bson
} // namespace pot

#endif
