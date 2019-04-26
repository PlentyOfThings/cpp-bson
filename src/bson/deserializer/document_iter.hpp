#ifndef POT_BSON_DESERIALIZER_DOCUMENT_ITER_H_
#define POT_BSON_DESERIALIZER_DOCUMENT_ITER_H_

#include "../consts.hpp"
#include "./document.hpp"
#include <cstdlib>

namespace pot {
namespace bson {
namespace deserializer {

class DocumentIter {
public:
  DocumentIter(const Document &doc) : doc_(doc) {
    current_ = doc.offset_ + static_cast<uint8_t>(TypeSize::Int32);
  }

  DocumentIter(const Document &doc, size_t current) :
      doc_(doc), current_(current) {}

  bool operator==(DocumentIter &other) const {
    return &doc_ == &other.doc_ && current_ == other.current_;
  }

  bool operator!=(DocumentIter &other) const {
    return !(*this == other);
  }

  DocumentElement operator*() const {
    return { doc_.buffer_, current_, doc_.buffer_length_ };
  }

  DocumentIter &operator++() {
    DocumentElement el = **this;
    current_ +=
        // Size of the type byte.
        static_cast<uint8_t>(TypeSize::Byte) +
        // Size of the name string.
        el.nameSize() +
        // Size of the data in the element.
        el.dataSize();
    return *this;
  }

  DocumentIter operator++(int) {
    DocumentIter copy(*this);
    ++*this;
    return copy;
  }

private:
  const Document &doc_;
  size_t current_;
};

Document::iterator Document::begin() const {
  return Document::iterator(*this);
}

Document::iterator Document::end() const {
  return Document::iterator(*this, this->offset_ + this->len() - 1);
}

} // namespace deserializer
} // namespace bson
} // namespace pot

#endif
