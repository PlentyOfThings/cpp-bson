#include "../src/bson.h"
#include "./utils.hpp"
#include "cxxtest/TestSuite.h"

namespace bsons = pot::bson::serializer;

static constexpr size_t kBufSize = 256;

class SerializerTests : public CxxTest::TestSuite {
  uint8_t buf[kBufSize];

public:
  void setUp() {
    clear_buf(buf, kBufSize);
  }

  void testEmptyDocument() {
    bsons::Result res =
        bsons::Document::build(buf, kBufSize, [](bsons::Document &doc) {});

    uint8_t expected[kBufSize] = { 0x05, 0x00, 0x00, 0x00, 0x00 };
    clear_buf(expected, 5, kBufSize);

    TS_ASSERT_SAME_DATA(buf, expected, kBufSize);
    TS_ASSERT_EQUALS(res.status, bsons::Status::Ok);
    TS_ASSERT_EQUALS(res.len, 5);
  }

  void testSimpleDocumentDouble() {
    bsons::Result res =
        bsons::Document::build(buf, kBufSize, [](bsons::Document &doc) {
          doc.appendDouble("a", 0.2);
        });

    uint8_t expected[kBufSize] = {
      0x10, 0x00, 0x00, 0x00, 0x01, 0x61, 0x00, 0x9A,
      0x99, 0x99, 0x99, 0x99, 0x99, 0xC9, 0x3F, 0x00,
    };
    clear_buf(expected, 16, kBufSize);

    TS_ASSERT_SAME_DATA(buf, expected, kBufSize);
    TS_ASSERT_EQUALS(res.status, bsons::Status::Ok);
    TS_ASSERT_EQUALS(res.len, 16);
  }

  void testSimpleDocumentString() {
    bsons::Result res =
        bsons::Document::build(buf, kBufSize, [](bsons::Document &doc) {
          doc.appendStr("hello", "world");
        });

    uint8_t expected[kBufSize] = {
      0x16, 0x00, 0x00, 0x00, 0x02, 0x68, 0x65, 0x6C, 0x6C, 0x6F, 0x00,
      0x06, 0x00, 0x00, 0x00, 0x77, 0x6F, 0x72, 0x6C, 0x64, 0x00, 0x00,
    };
    clear_buf(expected, 22, kBufSize);

    TS_ASSERT_SAME_DATA(buf, expected, kBufSize);
    TS_ASSERT_EQUALS(res.status, bsons::Status::Ok);
    TS_ASSERT_EQUALS(res.len, 22);
  }

  void testSimpleNestedDocument() {
    bsons::Result res =
        bsons::Document::build(buf, kBufSize, [](bsons::Document &doc) {
          doc.appendDoc("a", [](bsons::Document &internalDoc) {
            internalDoc.appendStr("b", "c");
          });
        });

    uint8_t expected[kBufSize] = {
      0x16, 0x00, 0x00, 0x00, 0x03, 0x61, 0x00, 0x0E, 0x00, 0x00, 0x00,
      0x02, 0x62, 0x00, 0x02, 0x00, 0x00, 0x00, 0x63, 0x00, 0x00, 0x00,
    };
    clear_buf(expected, 22, kBufSize);

    TS_ASSERT_SAME_DATA(buf, expected, kBufSize);
    TS_ASSERT_EQUALS(res.status, bsons::Status::Ok);
    TS_ASSERT_EQUALS(res.len, 22);
  }

  void testSimpleDocumentBinary() {
    bsons::Result res =
        bsons::Document::build(buf, kBufSize, [](bsons::Document &doc) {
          uint8_t bin[] = { 1, 2, 3, 2, 1 };
          doc.appendBinary("a", bin, 5);
        });

    uint8_t expected[kBufSize] = {
      0x12, 0x00, 0x00, 0x00, 0x05, 0x61, 0x00, 0x05, 0x00,
      0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x02, 0x01, 0x00,
    };
    clear_buf(expected, 18, kBufSize);

    TS_ASSERT_SAME_DATA(buf, expected, kBufSize);
    TS_ASSERT_EQUALS(res.status, bsons::Status::Ok);
    TS_ASSERT_EQUALS(res.len, 18);
  }

  void testSimpleDocumentBool() {
    bsons::Result res =
        bsons::Document::build(buf, kBufSize, [](bsons::Document &doc) {
          doc.appendBool("true", true);
          doc.appendBool("false", false);
        });

    uint8_t expected[kBufSize] = {
      0x14, 0x00, 0x00, 0x00, 0x08, 0x74, 0x72, 0x75, 0x65, 0x00,
      0x01, 0x08, 0x66, 0x61, 0x6C, 0x73, 0x65, 0x00, 0x00, 0x00,
    };
    clear_buf(expected, 20, kBufSize);

    TS_ASSERT_SAME_DATA(buf, expected, kBufSize);
    TS_ASSERT_EQUALS(res.status, bsons::Status::Ok);
    TS_ASSERT_EQUALS(res.len, 20);
  }

  void testSimpleDocumentNull() {
    bsons::Result res = bsons::Document::build(
        buf, kBufSize, [](bsons::Document &doc) { doc.appendNull("nil"); });

    uint8_t expected[kBufSize] = {
      0x0A, 0x00, 0x00, 0x00, 0x0A, 0x6E, 0x69, 0x6C, 0x00, 0x00,
    };
    clear_buf(expected, 10, kBufSize);

    TS_ASSERT_SAME_DATA(buf, expected, kBufSize);
    TS_ASSERT_EQUALS(res.status, bsons::Status::Ok);
    TS_ASSERT_EQUALS(res.len, 10);
  }

  void testSimpleDocumentInt32() {
    bsons::Result res =
        bsons::Document::build(buf, kBufSize, [](bsons::Document &doc) {
          doc.appendInt32("val", 9812);
        });

    uint8_t expected[kBufSize] = {
      0x0E, 0x00, 0x00, 0x00, 0x10, 0x76, 0x61,
      0x6C, 0x00, 0x54, 0x26, 0x00, 0x00, 0x00,
    };
    clear_buf(expected, 14, kBufSize);

    TS_ASSERT_SAME_DATA(buf, expected, kBufSize);
    TS_ASSERT_EQUALS(res.status, bsons::Status::Ok);
    TS_ASSERT_EQUALS(res.len, 14);
  }

  void testSimpleDocumentInt64() {
    bsons::Result res =
        bsons::Document::build(buf, kBufSize, [](bsons::Document &doc) {
          doc.appendInt64("val", 98761234);
        });

    uint8_t expected[kBufSize] = {
      0x12, 0x00, 0x00, 0x00, 0x12, 0x76, 0x61, 0x6C, 0x00,
      0x12, 0xFA, 0xE2, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00,
    };
    clear_buf(expected, 18, kBufSize);

    TS_ASSERT_SAME_DATA(buf, expected, kBufSize);
    TS_ASSERT_EQUALS(res.status, bsons::Status::Ok);
    TS_ASSERT_EQUALS(res.len, 18);
  }

  void testArrayAppend() {
    bsons::Result res =
        bsons::Document::build(buf, kBufSize, [](bsons::Document &doc) {
          doc.appendArray("arr", [](bsons::Document &darr) {
            uint8_t bin[] = { 1, 2, 3 };

            bsons::Array arr(&darr);
            arr.appendDouble(0.2)
                .appendStr("element")
                .appendDoc(
                    [](bsons::Document &nested) { nested.appendStr("a", "b"); })
                .appendBinary(bin, 3)
                .appendBool(true)
                .appendBool(false)
                .appendNull()
                .appendInt32(21)
                .appendInt64(91);
          });
        });

    uint8_t expected[kBufSize] = {
      0x62, 0x00, 0x00, 0x00, 0x04, 0x61, 0x72, 0x72, 0x00, 0x58, 0x00,
      0x00, 0x00, 0x01, 0x30, 0x00, 0x9A, 0x99, 0x99, 0x99, 0x99, 0x99,
      0xC9, 0x3F, 0x02, 0x31, 0x00, 0x08, 0x00, 0x00, 0x00, 0x65, 0x6C,
      0x65, 0x6D, 0x65, 0x6E, 0x74, 0x00, 0x03, 0x32, 0x00, 0x0E, 0x00,
      0x00, 0x00, 0x02, 0x61, 0x00, 0x02, 0x00, 0x00, 0x00, 0x62, 0x00,
      0x00, 0x05, 0x33, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02,
      0x03, 0x08, 0x34, 0x00, 0x01, 0x08, 0x35, 0x00, 0x00, 0x0A, 0x36,
      0x00, 0x10, 0x37, 0x00, 0x15, 0x00, 0x00, 0x00, 0x12, 0x38, 0x00,
      0x5B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };
    clear_buf(expected, 98, kBufSize);

    TS_ASSERT_SAME_DATA(buf, expected, kBufSize);
    TS_ASSERT_EQUALS(res.status, bsons::Status::Ok);
    TS_ASSERT_EQUALS(res.len, 98);
  }
};
