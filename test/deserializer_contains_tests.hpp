#include "../src/bson/bson.hpp"
#include "cxxtest/TestSuite.h"

namespace bsond = pot::bson::deserializer;

class DeserializerContainsTests : public CxxTest::TestSuite {
public:
  void testGetElement() {
    uint8_t buf[] = {
      0x16, 0x00, 0x00, 0x00, 0x04, 0x61, 0x72, 0x72, 0x00, 0x0C, 0x00,
      0x00, 0x00, 0x10, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };
    bsond::Document doc(buf, sizeof(buf));

    auto arrEl = doc.getElByName("arr");
    TS_ASSERT(arrEl);

    TS_ASSERT_EQUALS(arrEl->type(), pot::bson::Element::Array);
    auto arr = arrEl->getArr();

    auto first = arr.getElByIndex(0);
    TS_ASSERT(first);
    TS_ASSERT_EQUALS(first->type(), pot::bson::Element::Int32);

    auto firstByStr = arr.getElByName("0");
    TS_ASSERT(firstByStr);
    TS_ASSERT_EQUALS(firstByStr->type(), pot::bson::Element::Int32);
    TS_ASSERT_EQUALS(first->getInt32(), firstByStr->getInt32());

    auto missing = doc.getElByName("missing");
    TS_ASSERT(!missing);
  }

  void testDouble() {
    uint8_t buf[] = {
      0x30, 0x00, 0x00, 0x00, 0x04, 0x61, 0x72, 0x72, 0x00, 0x26, 0x00, 0x00,
      0x00, 0x01, 0x30, 0x00, 0x9A, 0x99, 0x99, 0x99, 0x99, 0x99, 0xC9, 0x3F,
      0x01, 0x31, 0x00, 0x9A, 0x99, 0x99, 0x99, 0x99, 0x99, 0xD9, 0x3F, 0x01,
      0x32, 0x00, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0xE3, 0x3F, 0x00, 0x00,
    };
    bsond::Document doc(buf, sizeof(buf));

    auto arr = doc.getElByName("arr");
    TS_ASSERT(arr);
    TS_ASSERT_EQUALS(arr->type(), pot::bson::Element::Array);
    TS_ASSERT_EQUALS(arr->getArr().containsDouble(0.2, 0.0001), true);
    TS_ASSERT_EQUALS(arr->getArr().containsDouble(0.16, 0.0001), false);
  }

  void testStr() {
    uint8_t buf[] = {
      0x1D, 0x00, 0x00, 0x00, 0x04, 0x61, 0x72, 0x72, 0x00, 0x13,
      0x00, 0x00, 0x00, 0x02, 0x30, 0x00, 0x07, 0x00, 0x00, 0x00,
      0x65, 0x78, 0x69, 0x73, 0x74, 0x73, 0x00, 0x00, 0x00,
    };
    bsond::Document doc(buf, sizeof(buf));

    auto arr = doc.getElByName("arr");
    TS_ASSERT(arr);
    TS_ASSERT_EQUALS(arr->type(), pot::bson::Element::Array);
    TS_ASSERT_EQUALS(arr->getArr().containsStr("exists"), true);
    TS_ASSERT_EQUALS(arr->getArr().containsStr("missing"), false);
  }

  void testBool() {
    uint8_t buf[] = {
      0x13, 0x00, 0x00, 0x00, 0x04, 0x61, 0x72, 0x72, 0x00, 0x09,
      0x00, 0x00, 0x00, 0x08, 0x30, 0x00, 0x01, 0x00, 0x00,
    };
    bsond::Document doc(buf, sizeof(buf));

    auto arr = doc.getElByName("arr");
    TS_ASSERT(arr);
    TS_ASSERT_EQUALS(arr->type(), pot::bson::Element::Array);
    TS_ASSERT_EQUALS(arr->getArr().containsBool(true), true);
    TS_ASSERT_EQUALS(arr->getArr().containsBool(false), false);
  }

  void testNullExists() {
    uint8_t buf[] = {
      0x1B, 0x00, 0x00, 0x00, 0x04, 0x61, 0x72, 0x72, 0x00,
      0x11, 0x00, 0x00, 0x00, 0x02, 0x30, 0x00, 0x02, 0x00,
      0x00, 0x00, 0x61, 0x00, 0x0A, 0x31, 0x00, 0x00, 0x00,
    };
    bsond::Document doc(buf, sizeof(buf));

    auto arr = doc.getElByName("arr");
    TS_ASSERT(arr);
    TS_ASSERT_EQUALS(arr->type(), pot::bson::Element::Array);
    TS_ASSERT_EQUALS(arr->getArr().containsNull(), true);
  }

  void testNullMissing() {
    uint8_t buf[] = {
      0x18, 0x00, 0x00, 0x00, 0x04, 0x61, 0x72, 0x72, 0x00, 0x0E, 0x00, 0x00,
      0x00, 0x02, 0x30, 0x00, 0x02, 0x00, 0x00, 0x00, 0x61, 0x00, 0x00, 0x00,
    };
    bsond::Document doc(buf, sizeof(buf));

    auto arr = doc.getElByName("arr");
    TS_ASSERT(arr);
    TS_ASSERT_EQUALS(arr->type(), pot::bson::Element::Array);
    TS_ASSERT_EQUALS(arr->getArr().containsNull(), false);
  }

  void testInt32() {
    uint8_t buf[] = {
      0x16, 0x00, 0x00, 0x00, 0x04, 0x61, 0x72, 0x72, 0x00, 0x0C, 0x00,
      0x00, 0x00, 0x10, 0x30, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00,
    };
    bsond::Document doc(buf, sizeof(buf));

    auto arr = doc.getElByName("arr");
    TS_ASSERT(arr);
    TS_ASSERT_EQUALS(arr->type(), pot::bson::Element::Array);
    TS_ASSERT_EQUALS(arr->getArr().containsInt32(10), true);
    TS_ASSERT_EQUALS(arr->getArr().containsInt32(15), false);
  }

  void testInt64() {
    uint8_t buf[] = {
      0x1A, 0x00, 0x00, 0x00, 0x04, 0x61, 0x72, 0x72, 0x00,
      0x10, 0x00, 0x00, 0x00, 0x12, 0x30, 0x00, 0x0A, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };
    bsond::Document doc(buf, sizeof(buf));

    auto arr = doc.getElByName("arr");
    TS_ASSERT(arr);
    TS_ASSERT_EQUALS(arr->type(), pot::bson::Element::Array);
    TS_ASSERT_EQUALS(arr->getArr().containsInt64(10), true);
    TS_ASSERT_EQUALS(arr->getArr().containsInt64(15), false);
  }
};
