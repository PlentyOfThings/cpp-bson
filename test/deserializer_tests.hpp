#include "../src/bson/bson.hpp"
#include "cxxtest/TestSuite.h"

namespace bsond = pot::bson::deserializer;

class DeserializerTests : public CxxTest::TestSuite {
public:
  void testEmptyDocument() {
    uint8_t buf[] = { 0x05, 0x00, 0x00, 0x00, 0x00 };
    bsond::Document doc(buf, sizeof(buf));

    int iters = 0;
    for (auto const &el : doc) {
      (void)el;
      iters++;
    }

    TS_ASSERT_EQUALS(iters, 0);
  }

  void testDouble() {
    uint8_t buf[] = {
      0x10, 0x00, 0x00, 0x00, 0x01, 0x61, 0x00, 0x9A,
      0x99, 0x99, 0x99, 0x99, 0x99, 0xC9, 0x3F, 0x00,
    };
    bsond::Document doc(buf, sizeof(buf));

    int iters = 0;
    bool brk = false;
    for (auto const &el : doc) {
      switch (iters) {
        case 0: {
          TS_ASSERT_EQUALS(el.type(), pot::bson::Element::Double);
          TS_ASSERT(el.nameEquals("a"));
          TS_ASSERT_EQUALS(el.getDouble(), 0.2);
          break;
        }
        default: {
          brk = true;
          break;
        }
      }

      iters++;
      if (brk) {
        break;
      }
    }

    TS_ASSERT_EQUALS(iters, 1);
  }

  void testStr() {
    uint8_t buf[] = {
      0x16, 0x00, 0x00, 0x00, 0x02, 0x68, 0x65, 0x6C, 0x6C, 0x6F, 0x00,
      0x06, 0x00, 0x00, 0x00, 0x77, 0x6F, 0x72, 0x6C, 0x64, 0x00, 0x00,
    };
    bsond::Document doc(buf, sizeof(buf));

    int iters = 0;
    bool brk = false;
    for (auto const &el : doc) {
      switch (iters) {
        case 0: {
          TS_ASSERT_EQUALS(el.type(), pot::bson::Element::String);

          {
            char name[6];
            TS_ASSERT_EQUALS(el.getName(name, sizeof(name)), 6);
            TS_ASSERT_SAME_DATA(name, "hello", sizeof(name));
            TS_ASSERT(el.nameEquals("hello"));
          }

          {
            char name[4];
            TS_ASSERT_EQUALS(el.getName(name, sizeof(name)), 4);
            TS_ASSERT_SAME_DATA(name, "hel", sizeof(name));
          }

          {
            char name[8];
            TS_ASSERT_EQUALS(el.getName(name, sizeof(name)), 6);
            TS_ASSERT_SAME_DATA(name, "hello", 6);
          }

          {
            char value[6];
            TS_ASSERT_EQUALS(el.getStr(value, sizeof(value)), 6);
            TS_ASSERT_SAME_DATA(value, "world", sizeof(value));
            TS_ASSERT(el.strEquals("world"));
          }

          {
            char value[4];
            TS_ASSERT_EQUALS(el.getStr(value, sizeof(value)), 4);
            TS_ASSERT_SAME_DATA(value, "wor", sizeof(value));
          }

          {
            char value[8];
            TS_ASSERT_EQUALS(el.getStr(value, sizeof(value)), 6);
            TS_ASSERT_SAME_DATA(value, "world", 6);
          }
          break;
        }
        default: {
          brk = true;
          break;
        }
      }

      iters++;
      if (brk) {
        break;
      }
    }

    TS_ASSERT_EQUALS(iters, 1);
  }

  void testNestedDoc() {
    uint8_t buf[] = {
      0x16, 0x00, 0x00, 0x00, 0x03, 0x61, 0x00, 0x0E, 0x00, 0x00, 0x00,
      0x02, 0x62, 0x00, 0x02, 0x00, 0x00, 0x00, 0x63, 0x00, 0x00, 0x00,
    };
    bsond::Document doc(buf, sizeof(buf));

    int iters = 0;
    bool brk = false;
    for (auto const &el : doc) {
      switch (iters) {
        case 0: {
          TS_ASSERT_EQUALS(el.type(), pot::bson::Element::Document);
          TS_ASSERT(el.nameEquals("a"));

          const auto nested = el.getDoc();
          int niters = 0;
          bool nbrk = false;
          for (auto const &nel : nested) {
            switch (niters) {
              case 0: {
                TS_ASSERT(nel.nameEquals("b"));
                TS_ASSERT(nel.strEquals("c"));
              }
              default: {
                nbrk = true;
                break;
              }
            }

            niters++;
            if (nbrk) {
              break;
            }
          }
          break;
        }
        default: {
          brk = true;
          break;
        }
      }

      iters++;
      if (brk) {
        break;
      }
    }

    TS_ASSERT_EQUALS(iters, 1);
  }

  void testBinary() {
    uint8_t buf[] = {
      0x12, 0x00, 0x00, 0x00, 0x05, 0x61, 0x00, 0x05, 0x00,
      0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x02, 0x01, 0x00,
    };
    bsond::Document doc(buf, sizeof(buf));

    int iters = 0;
    bool brk = false;
    for (auto const &el : doc) {
      switch (iters) {
        case 0: {
          TS_ASSERT_EQUALS(el.type(), pot::bson::Element::Binary);
          TS_ASSERT(el.nameEquals("a"));

          uint8_t expected[] = { 1, 2, 3, 2, 1 };

          {
            uint8_t buf[5];
            TS_ASSERT_EQUALS(el.getBinary(buf, sizeof(buf)), 5);
            TS_ASSERT_SAME_DATA(buf, expected, sizeof(buf));
          }

          {
            uint8_t buf[3];
            TS_ASSERT_EQUALS(el.getBinary(buf, sizeof(buf)), 3);
            TS_ASSERT_SAME_DATA(buf, expected, sizeof(buf));
          }

          {
            uint8_t buf[7];
            TS_ASSERT_EQUALS(el.getBinary(buf, sizeof(buf)), 5);
            TS_ASSERT_SAME_DATA(buf, expected, 5);
          }
          break;
        }
        default: {
          brk = true;
          break;
        }
      }

      iters++;
      if (brk) {
        break;
      }
    }

    TS_ASSERT_EQUALS(iters, 1);
  }

  void testBoolean() {
    uint8_t buf[] = {
      0x14, 0x00, 0x00, 0x00, 0x08, 0x74, 0x72, 0x75, 0x65, 0x00,
      0x01, 0x08, 0x66, 0x61, 0x6C, 0x73, 0x65, 0x00, 0x00, 0x00,
    };
    bsond::Document doc(buf, sizeof(buf));

    int iters = 0;
    bool brk = false;
    for (auto const &el : doc) {
      switch (iters) {
        case 0: {
          TS_ASSERT_EQUALS(el.type(), pot::bson::Element::Boolean);
          TS_ASSERT(el.nameEquals("true"));
          TS_ASSERT_EQUALS(el.getBool(), true);
          break;
        }
        case 1: {
          TS_ASSERT_EQUALS(el.type(), pot::bson::Element::Boolean);
          TS_ASSERT(el.nameEquals("false"));
          TS_ASSERT_EQUALS(el.getBool(), false);
          break;
        }
        default: {
          brk = true;
          break;
        }
      }

      iters++;
      if (brk) {
        break;
      }
    }

    TS_ASSERT_EQUALS(iters, 2);
  }

  void testNull() {
    uint8_t buf[] = {
      0x0A, 0x00, 0x00, 0x00, 0x0A, 0x6E, 0x69, 0x6C, 0x00, 0x00,
    };
    bsond::Document doc(buf, sizeof(buf));

    int iters = 0;
    bool brk = false;
    for (auto const &el : doc) {
      switch (iters) {
        case 0: {
          TS_ASSERT_EQUALS(el.type(), pot::bson::Element::Null);
          TS_ASSERT(el.nameEquals("nil"));
          break;
        }
        default: {
          brk = true;
          break;
        }
      }

      iters++;
      if (brk) {
        break;
      }
    }

    TS_ASSERT_EQUALS(iters, 1);
  }

  void testInt32() {
    uint8_t buf[] = {
      0x0E, 0x00, 0x00, 0x00, 0x10, 0x76, 0x61,
      0x6C, 0x00, 0x54, 0x26, 0x00, 0x00, 0x00,
    };
    bsond::Document doc(buf, sizeof(buf));

    int iters = 0;
    bool brk = false;
    for (auto const &el : doc) {
      switch (iters) {
        case 0: {
          TS_ASSERT_EQUALS(el.type(), pot::bson::Element::Int32);
          TS_ASSERT(el.nameEquals("val"));
          TS_ASSERT_EQUALS(el.getInt32(), 9812);
          break;
        }
        default: {
          brk = true;
          break;
        }
      }

      iters++;
      if (brk) {
        break;
      }
    }

    TS_ASSERT_EQUALS(iters, 1);
  }

  void testInt64() {
    uint8_t buf[] = {
      0x12, 0x00, 0x00, 0x00, 0x12, 0x76, 0x61, 0x6C, 0x00,
      0x12, 0xFA, 0xE2, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00,
    };
    bsond::Document doc(buf, sizeof(buf));

    int iters = 0;
    bool brk = false;
    for (auto const &el : doc) {
      switch (iters) {
        case 0: {
          TS_ASSERT_EQUALS(el.type(), pot::bson::Element::Int64);
          TS_ASSERT(el.nameEquals("val"));
          TS_ASSERT_EQUALS(el.getInt64(), 98761234);
          break;
        }
        default: {
          brk = true;
          break;
        }
      }

      iters++;
      if (brk) {
        break;
      }
    }

    TS_ASSERT_EQUALS(iters, 1);
  }
};
