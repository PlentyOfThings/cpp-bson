#ifndef POT_BSON_DESERIALIZER_DOCUMENT_ITER_H_
#define POT_BSON_DESERIALIZER_DOCUMENT_ITER_H_

#include "../consts.hpp"
#include "./document.hpp"
#include <cstdlib>

namespace pot {
namespace bson {
namespace deserializer {

template <class Element> class DocumentIter {
public:
  DocumentIter(const Document &doc) : doc_(doc) {
    this->current_ = doc.offset_ + static_cast<uint8_t>(TypeSize::Int32);
  }

  DocumentIter(const Document &doc, size_t current) :
      doc_(doc), current_(current) {}

  bool operator==(DocumentIter &other) const {
    return &this->doc_ == &other.doc_ && this->current_ == other.current_;
  }

  bool operator!=(DocumentIter &other) const {
    return !(*this == other);
  }

  virtual Element operator*() const {
    return { this->doc_.buffer_, this->current_, this->doc_.buffer_length_ };
  }

  virtual DocumentIter &operator++() {
    Element el = **this;
    current_ +=
        // Size of the type byte.
        static_cast<uint8_t>(TypeSize::Byte) +
        // Size of the name string.
        el.nameSize() +
        // Size of the data in the element.
        el.dataSize();
    return *this;
  }

protected:
  const Document &doc_;
  size_t current_;
};

Document::iterator Document::begin() const {
  return Document::iterator(*this);
}

Document::iterator Document::end() const {
  return Document::iterator(*this, this->offset_ + this->len() - 1);
}

bool Document::getElByName(const char name[], DocumentElement &out) const {
  for (auto const &el : *this) {
    if (el.nameEquals(name)) {
      out = el;
      return true;
    }
  }

  return false;
}

} // namespace deserializer
} // namespace bson
} // namespace pot

#endif
