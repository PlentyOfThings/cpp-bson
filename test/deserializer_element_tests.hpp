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

    bsond::DocumentElement arrEl;
    TS_ASSERT(doc.getElByName("arr", arrEl));

    TS_ASSERT_EQUALS(arrEl.type(), pot::bson::Element::Array);
    auto arr = arrEl.getArr();

    bsond::ArrayElement first;
    TS_ASSERT(arr.getElByIndex(0, first));
    TS_ASSERT_EQUALS(first.type(), pot::bson::Element::Int32);

    bsond::ArrayElement firstByStr;
    TS_ASSERT(arr.getElByName("0", firstByStr));
    TS_ASSERT_EQUALS(firstByStr.type(), pot::bson::Element::Int32);
    TS_ASSERT_EQUALS(first.getInt32(), firstByStr.getInt32());

    bsond::DocumentElement missing;
    TS_ASSERT(!doc.getElByName("missing", missing));
  }

  void testGetNumber() {
    uint8_t buf[] = {
      0x26, 0x00, 0x00, 0x00, 0x10, 0x33, 0x32, 0x00, 0x20, 0x00,
      0x00, 0x00, 0x12, 0x36, 0x34, 0x00, 0x40, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x01, 0x64, 0x62, 0x6C, 0x00, 0x9A,
      0x99, 0x99, 0x99, 0x99, 0x99, 0xC9, 0x3F, 0x00,
    };
    bsond::Document doc(buf, sizeof(buf));

    bsond::DocumentElement dbl;
    TS_ASSERT(doc.getElByName("dbl", dbl));
    TS_ASSERT_EQUALS(dbl.getNumber(), 0.2);
    TS_ASSERT_EQUALS(dbl.isNumber(), true);
    TS_ASSERT_EQUALS(dbl.isInt(), false);

    bsond::DocumentElement i32;
    TS_ASSERT(doc.getElByName("32", i32));
    TS_ASSERT_EQUALS(i32.getNumber(), 32);
    TS_ASSERT_EQUALS(i32.isNumber(), true);
    TS_ASSERT_EQUALS(i32.isInt(), true);

    bsond::DocumentElement i64;
    TS_ASSERT(doc.getElByName("64", i64));
    TS_ASSERT_EQUALS(i64.getNumber(), 64);
    TS_ASSERT_EQUALS(i32.isNumber(), true);
    TS_ASSERT_EQUALS(i32.isInt(), true);
  }

  void testGetInt() {
    uint8_t buf[] = {
      0x19, 0x00, 0x00, 0x00, 0x10, 0x33, 0x32, 0x00, 0x20,
      0x00, 0x00, 0x00, 0x12, 0x36, 0x34, 0x00, 0x40, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };
    bsond::Document doc(buf, sizeof(buf));

    bsond::DocumentElement i32;
    TS_ASSERT(doc.getElByName("32", i32));
    TS_ASSERT_EQUALS(i32.getInt(), 32);

    bsond::DocumentElement i64;
    TS_ASSERT(doc.getElByName("64", i64));
    TS_ASSERT_EQUALS(i64.getInt(), 64);
  }
};
