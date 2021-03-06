#include "../src/bson/bson.hpp"
#include "./utils.hpp"
#include "cxxtest/TestSuite.h"

namespace bsons = pot::bson::serializer;

static constexpr size_t kBufSize = 256;

class SerializerTests : public CxxTest::TestSuite {
  uint8_t buf[kBufSize];
  bsons::Document *rootDoc;

public:
  void setUp() {
    clear_buf(buf, kBufSize);
    rootDoc = new bsons::Document(buf, kBufSize);
  }

  void tearDown() {
    delete rootDoc;
  }

  void testEmptyDocument() {
    bsons::Result res = rootDoc->end();

    uint8_t expected[kBufSize] = { 0x05, 0x00, 0x00, 0x00, 0x00 };
    clear_buf(expected, 5, kBufSize);

    TS_ASSERT_SAME_DATA(buf, expected, kBufSize);
    TS_ASSERT_EQUALS(res.status, bsons::Status::Ok);
    TS_ASSERT_EQUALS(res.len, 5);
  }

  void testSimpleDocumentDouble() {
    bsons::Result res = rootDoc->appendDouble("a", 0.2).end();

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
    bsons::Result res = rootDoc->appendStr("hello", "world").end();

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
    {
      bsons::Document nested("a", rootDoc);
      nested.appendStr("b", "c");
    }

    bsons::Result res = rootDoc->end();

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
    uint8_t bin[] = { 1, 2, 3, 2, 1 };
    bsons::Result res = rootDoc->appendBin("a", bin, 5).end();

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
        rootDoc->appendBool("true", true).appendBool("false", false).end();

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
    bsons::Result res = rootDoc->appendNull("nil").end();

    uint8_t expected[kBufSize] = {
      0x0A, 0x00, 0x00, 0x00, 0x0A, 0x6E, 0x69, 0x6C, 0x00, 0x00,
    };
    clear_buf(expected, 10, kBufSize);

    TS_ASSERT_SAME_DATA(buf, expected, kBufSize);
    TS_ASSERT_EQUALS(res.status, bsons::Status::Ok);
    TS_ASSERT_EQUALS(res.len, 10);
  }

  void testSimpleDocumentInt32() {
    bsons::Result res = rootDoc->appendInt32("val", 9812).end();

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
    bsons::Result res = rootDoc->appendInt64("val", 98761234).end();

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
    {
      bsons::Array arr("arr", rootDoc);

      arr.appendDouble(0.2).appendStr("element");

      {
        bsons::Document nested(arr);
        nested.appendStr("a", "b");
      }

      uint8_t bin[] = { 1, 2, 3 };
      arr.appendBin(bin, 3)
          .appendBool(true)
          .appendBool(false)
          .appendNull()
          .appendInt32(21)
          .appendInt64(91);
    }

    bsons::Result res = rootDoc->end();

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

  void testComplexDocument() {
    uint8_t bin[] = { 4, 3, 2, 1 };

    bsons::Result res =
        bsons::Document::build(buf, kBufSize, [bin](bsons::Document &doc) {
          doc.appendDouble("dbl", 0.4)
              .appendStr("str", "string")
              .appendDoc("doc",
                         [](bsons::Document &ndoc) {
                           ndoc.appendStr("this", "is")
                               .appendStr("a", "nested")
                               .appendStr("doc", "ument");
                         })
              .appendBin("buf", bin, 4)
              .appendDoc("bools",
                         [](bsons::Document &ndoc) {
                           ndoc.appendBool("t", true).appendBool("f", false);
                         })
              .appendNull("nil")
              .appendDoc(
                  "ints",
                  [](bsons::Document &ndoc) {
                    ndoc.appendInt32("32", 9876).appendInt64("64", 987654321);
                  })
              .appendArr("arr", [bin](bsons::Array &narr) {
                narr.appendDouble(0.2)
                    .appendStr("element")
                    .appendDoc(
                        [](bsons::Document &ndoc) { ndoc.appendStr("a", "b"); })
                    .appendBin(bin, 4)
                    .appendBool(true)
                    .appendBool(false)
                    .appendNull()
                    .appendInt32(21)
                    .appendInt64(91);
              });
        });

    uint8_t expected[kBufSize] = {
      0xFA, 0x00, 0x00, 0x00, 0x01, 0x64, 0x62, 0x6C, 0x00, 0x9A, 0x99, 0x99,
      0x99, 0x99, 0x99, 0xD9, 0x3F, 0x02, 0x73, 0x74, 0x72, 0x00, 0x07, 0x00,
      0x00, 0x00, 0x73, 0x74, 0x72, 0x69, 0x6E, 0x67, 0x00, 0x03, 0x64, 0x6F,
      0x63, 0x00, 0x2F, 0x00, 0x00, 0x00, 0x02, 0x74, 0x68, 0x69, 0x73, 0x00,
      0x03, 0x00, 0x00, 0x00, 0x69, 0x73, 0x00, 0x02, 0x61, 0x00, 0x07, 0x00,
      0x00, 0x00, 0x6E, 0x65, 0x73, 0x74, 0x65, 0x64, 0x00, 0x02, 0x64, 0x6F,
      0x63, 0x00, 0x06, 0x00, 0x00, 0x00, 0x75, 0x6D, 0x65, 0x6E, 0x74, 0x00,
      0x00, 0x05, 0x62, 0x75, 0x66, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x04,
      0x03, 0x02, 0x01, 0x03, 0x62, 0x6F, 0x6F, 0x6C, 0x73, 0x00, 0x0D, 0x00,
      0x00, 0x00, 0x08, 0x74, 0x00, 0x01, 0x08, 0x66, 0x00, 0x00, 0x00, 0x0A,
      0x6E, 0x69, 0x6C, 0x00, 0x03, 0x69, 0x6E, 0x74, 0x73, 0x00, 0x19, 0x00,
      0x00, 0x00, 0x10, 0x33, 0x32, 0x00, 0x94, 0x26, 0x00, 0x00, 0x12, 0x36,
      0x34, 0x00, 0xB1, 0x68, 0xDE, 0x3A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04,
      0x61, 0x72, 0x72, 0x00, 0x59, 0x00, 0x00, 0x00, 0x01, 0x30, 0x00, 0x9A,
      0x99, 0x99, 0x99, 0x99, 0x99, 0xC9, 0x3F, 0x02, 0x31, 0x00, 0x08, 0x00,
      0x00, 0x00, 0x65, 0x6C, 0x65, 0x6D, 0x65, 0x6E, 0x74, 0x00, 0x03, 0x32,
      0x00, 0x0E, 0x00, 0x00, 0x00, 0x02, 0x61, 0x00, 0x02, 0x00, 0x00, 0x00,
      0x62, 0x00, 0x00, 0x05, 0x33, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x04,
      0x03, 0x02, 0x01, 0x08, 0x34, 0x00, 0x01, 0x08, 0x35, 0x00, 0x00, 0x0A,
      0x36, 0x00, 0x10, 0x37, 0x00, 0x15, 0x00, 0x00, 0x00, 0x12, 0x38, 0x00,
      0x5B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };
    clear_buf(expected, 250, kBufSize);

    TS_ASSERT_SAME_DATA(buf, expected, kBufSize);
    TS_ASSERT_EQUALS(res.status, bsons::Status::Ok);
    TS_ASSERT_EQUALS(res.len, 250);
  }

  void testBufferOverflow() {
    uint8_t smallBuf[5] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

    bsons::Result res =
        bsons::Document::build(smallBuf, 1, [](bsons::Document &doc) {});

    uint8_t expected[5] = { 0x05, 0xFF, 0xFF, 0xFF, 0xFF };

    TS_ASSERT_SAME_DATA(smallBuf, expected, 5);
    TS_ASSERT_EQUALS(res.status, bsons::Status::BufferOverflow);
    TS_ASSERT_EQUALS(res.len, 5);
  }
};
