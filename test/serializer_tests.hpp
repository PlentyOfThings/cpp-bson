#include "../src/bson.h"
#include "./utils.hpp"
#include "cxxtest/TestSuite.h"

namespace bsons = pot::bson::serializer;

static constexpr size_t kBufSize = 256;

class SerializerTests : public CxxTest::TestSuite {
public:
  void testEmptyDocument() {
    uint8_t buf[kBufSize];
    clear_buf(buf, kBufSize);

    bsons::Result res =
        bsons::Document::build(buf, kBufSize, [](bsons::Document &doc) {});

    uint8_t expected[kBufSize] = { 0x05, 0x00, 0x00, 0x00, 0x00 };
    clear_buf(expected, 5, kBufSize);

    TS_ASSERT_SAME_DATA(buf, expected, kBufSize);
    TS_ASSERT_EQUALS(res.status, bsons::Status::Ok);
    TS_ASSERT_EQUALS(res.len, 5);
  }

  void testSimpleDocumentDouble() {
    uint8_t buf[kBufSize];
    clear_buf(buf, kBufSize);

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
    uint8_t buf[kBufSize];
    clear_buf(buf, kBufSize);

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
};
