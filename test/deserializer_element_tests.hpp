#include "../src/bson/bson.hpp"
#include "cxxtest/TestSuite.h"

namespace bsond = pot::bson::deserializer;

class DeserializerElementTests : public CxxTest::TestSuite {
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

  void testGetNumber() {
    uint8_t buf[] = {
      0x26, 0x00, 0x00, 0x00, 0x10, 0x33, 0x32, 0x00, 0x20, 0x00,
      0x00, 0x00, 0x12, 0x36, 0x34, 0x00, 0x40, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x01, 0x64, 0x62, 0x6C, 0x00, 0x9A,
      0x99, 0x99, 0x99, 0x99, 0x99, 0xC9, 0x3F, 0x00,
    };
    bsond::Document doc(buf, sizeof(buf));

    auto dbl = doc.getElByName("dbl");
    TS_ASSERT(dbl);
    TS_ASSERT_EQUALS(dbl->getNumber(), 0.2);
    TS_ASSERT_EQUALS(dbl->isNumber(), true);
    TS_ASSERT_EQUALS(dbl->isInt(), false);

    auto i32 = doc.getElByName("32");
    TS_ASSERT(i32);
    TS_ASSERT_EQUALS(i32->getNumber(), 32);
    TS_ASSERT_EQUALS(i32->isNumber(), true);
    TS_ASSERT_EQUALS(i32->isInt(), true);

    auto i64 = doc.getElByName("64");
    TS_ASSERT(i64);
    TS_ASSERT_EQUALS(i64->getNumber(), 64);
    TS_ASSERT_EQUALS(i32->isNumber(), true);
    TS_ASSERT_EQUALS(i32->isInt(), true);
  }

  void testGetInt() {
    uint8_t buf[] = {
      0x19, 0x00, 0x00, 0x00, 0x10, 0x33, 0x32, 0x00, 0x20,
      0x00, 0x00, 0x00, 0x12, 0x36, 0x34, 0x00, 0x40, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };
    bsond::Document doc(buf, sizeof(buf));

    auto i32 = doc.getElByName("32");
    TS_ASSERT(i32);
    TS_ASSERT_EQUALS(i32->getInt(), 32);

    auto i64 = doc.getElByName("64");
    TS_ASSERT(i64);
    TS_ASSERT_EQUALS(i64->getInt(), 64);
  }
};
