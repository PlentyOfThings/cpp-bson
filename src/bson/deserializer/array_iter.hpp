#ifndef POT_BSON_DESERIALIZER_ARRAY_ITER_H_
#define POT_BSON_DESERIALIZER_ARRAY_ITER_H_

#include "../consts.hpp"
#include "./array.hpp"
#include "./document_iter.hpp"
#include <cmath>
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
    return { this->doc_.buffer_, this->current_, this->doc_.buffer_length_,
             index_ };
  }

  virtual ArrayIter &operator++() {
    index_++;
    this->DocumentIter<Element>::operator++();
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

bool Array::getElByName(const char index[], ArrayElement &out) const {
  for (auto const &el : *this) {
    if (el.nameEquals(index)) {
      out = el;
      return true;
    }
  }

  return false;
}

bool Array::getElByIndex(const size_t index, ArrayElement &out) const {
  for (auto const &el : *this) {
    if (el.getIndex() == index) {
      out = el;
      return true;
    }
  }

  return false;
}

bool Array::containsDouble(const double value, const double epsilon) const {
  for (auto const &el : *this) {
    if (el.type() == Element::Double &&
        fabs(el.getDouble() - value) < epsilon) {
      return true;
    }
  }

  return false;
}

bool Array::containsDouble(const double value) const {
  return this->containsDouble(value, std::numeric_limits<double>::epsilon());
}

bool Array::containsStr(const char str[]) const {
  for (auto const &el : *this) {
    if (el.type() == Element::String && el.strEquals(str)) {
      return true;
    }
  }

  return false;
}

bool Array::containsBool(const bool value) const {
  for (auto const &el : *this) {
    if (el.type() == Element::Boolean && el.getBool() == value) {
      return true;
    }
  }

  return false;
}

bool Array::containsNull() const {
  for (auto const &el : *this) {
    if (el.type() == Element::Null) {
      return true;
    }
  }

  return false;
}

bool Array::containsInt32(const int32_t value) const {
  for (auto const &el : *this) {
    if (el.type() == Element::Int32 && el.getInt32() == value) {
      return true;
    }
  }

  return false;
}

bool Array::containsInt64(const int64_t value) const {
  for (auto const &el : *this) {
    if (el.type() == Element::Int64 && el.getInt64() == value) {
      return true;
    }
  }

  return false;
}

bool Array::containsInt(const int64_t value) const {
  for (auto const &el : *this) {
    auto type = el.type();
    if ((type == Element::Int64 && el.getInt64() == value) ||
        (type == Element::Int32 && el.getInt32() == value)) {
      return true;
    }
  }

  return false;
}

bool Array::containsNumber(const double value, const double epsilon) const {
  for (auto const &el : *this) {
    auto type = el.type();
    if ((type == Element::Int64 && el.getInt64() == value) ||
        (type == Element::Int32 && el.getInt32() == value) ||
        (type == Element::Double && fabs(el.getDouble() - value) < epsilon)) {
      return true;
    }
  }

  return false;
}

bool Array::containsNumber(const double value) const {
  return this->containsNumber(value, std::numeric_limits<double>::epsilon());
}

} // namespace deserializer
} // namespace bson
} // namespace pot

#endif
