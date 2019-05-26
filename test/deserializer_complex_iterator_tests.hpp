#include "../src/bson/bson.hpp"
#include "cxxtest/TestSuite.h"

namespace bsond = pot::bson::deserializer;

class DeserializerComplexIteratorTests : public CxxTest::TestSuite {
public:
  void testNestedArr() {
    uint8_t buf[] = {
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
    bsond::Document doc(buf, sizeof(buf));

    int iters = 0;
    bool brk = false;
    for (auto const &el : doc) {
      switch (iters) {
        case 0: {
          TS_ASSERT_EQUALS(el.type(), pot::bson::Element::Double);
          TS_ASSERT(el.nameEquals("dbl"));
          TS_ASSERT_EQUALS(el.getDouble(), 0.4);
          break;
        }
        case 1: {
          TS_ASSERT_EQUALS(el.type(), pot::bson::Element::String);
          TS_ASSERT(el.nameEquals("str"));
          TS_ASSERT(el.strEquals("string"));
          break;
        }
        case 2: {
          TS_ASSERT_EQUALS(el.type(), pot::bson::Element::Document);
          TS_ASSERT(el.nameEquals("doc"));

          const auto nested = el.getDoc();
          int niters = 0;
          bool nbrk = false;
          for (auto const &nel : nested) {
            switch (niters) {
              case 0: {
                TS_ASSERT_EQUALS(nel.type(), pot::bson::Element::String);
                TS_ASSERT(nel.nameEquals("this"));
                TS_ASSERT(nel.strEquals("is"));
                break;
              }
              case 1: {
                TS_ASSERT_EQUALS(nel.type(), pot::bson::Element::String);
                TS_ASSERT(nel.nameEquals("a"));
                TS_ASSERT(nel.strEquals("nested"));
                break;
              }
              case 2: {
                TS_ASSERT_EQUALS(nel.type(), pot::bson::Element::String);
                TS_ASSERT(nel.nameEquals("doc"));
                TS_ASSERT(nel.strEquals("ument"));
                break;
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

          TS_ASSERT_EQUALS(niters, 3);
          break;
        }
        case 3: {
          TS_ASSERT_EQUALS(el.type(), pot::bson::Element::Binary);
          TS_ASSERT(el.nameEquals("buf"));

          uint8_t expected[] = { 4, 3, 2, 1 };
          uint8_t buf[sizeof(expected)];
          TS_ASSERT_EQUALS(el.getBin(buf, sizeof(expected)), sizeof(expected));
          TS_ASSERT_SAME_DATA(buf, expected, sizeof(expected));
          break;
        }
        case 4: {
          TS_ASSERT_EQUALS(el.type(), pot::bson::Element::Document);
          TS_ASSERT(el.nameEquals("bools"));

          const auto nested = el.getDoc();
          int niters = 0;
          bool nbrk = false;
          for (auto const &nel : nested) {
            switch (niters) {
              case 0: {
                TS_ASSERT_EQUALS(nel.type(), pot::bson::Element::Boolean);
                TS_ASSERT(nel.nameEquals("t"));
                TS_ASSERT_EQUALS(nel.getBool(), true);
                break;
              }
              case 1: {
                TS_ASSERT_EQUALS(nel.type(), pot::bson::Element::Boolean);
                TS_ASSERT(nel.nameEquals("f"));
                TS_ASSERT_EQUALS(nel.getBool(), false);
                break;
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

          TS_ASSERT_EQUALS(niters, 2);
          break;
        }
        case 5: {
          TS_ASSERT_EQUALS(el.type(), pot::bson::Element::Null);
          TS_ASSERT(el.nameEquals("nil"));
          break;
        }
        case 6: {
          TS_ASSERT_EQUALS(el.type(), pot::bson::Element::Document);
          TS_ASSERT(el.nameEquals("ints"));

          const auto nested = el.getDoc();
          int niters = 0;
          bool nbrk = false;
          for (auto const &nel : nested) {
            switch (niters) {
              case 0: {
                TS_ASSERT_EQUALS(nel.type(), pot::bson::Element::Int32);
                TS_ASSERT(nel.nameEquals("32"));
                TS_ASSERT_EQUALS(nel.getInt32(), 9876);
                break;
              }
              case 1: {
                TS_ASSERT_EQUALS(nel.type(), pot::bson::Element::Int64);
                TS_ASSERT(nel.nameEquals("64"));
                TS_ASSERT_EQUALS(nel.getInt64(), 987654321);
                break;
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

          TS_ASSERT_EQUALS(niters, 2);
          break;
        }
        case 7: {
          TS_ASSERT_EQUALS(el.type(), pot::bson::Element::Array);
          TS_ASSERT(el.nameEquals("arr"));

          const auto nested = el.getArr();
          int niters = 0;
          bool nbrk = false;
          for (auto const &nel : nested) {
            switch (niters) {
              case 0: {
                TS_ASSERT_EQUALS(nel.type(), pot::bson::Element::Double);
                TS_ASSERT(nel.nameEquals("0"));
                TS_ASSERT_EQUALS(nel.getIndex(), 0);
                TS_ASSERT_EQUALS(nel.getDouble(), 0.2);
                break;
              }
              case 1: {
                TS_ASSERT_EQUALS(nel.type(), pot::bson::Element::String);
                TS_ASSERT(nel.nameEquals("1"));
                TS_ASSERT_EQUALS(nel.getIndex(), 1);
                TS_ASSERT(nel.strEquals("element"));
                break;
              }
              case 2: {
                TS_ASSERT_EQUALS(nel.type(), pot::bson::Element::Document);
                TS_ASSERT(nel.nameEquals("2"));
                TS_ASSERT_EQUALS(nel.getIndex(), 2);

                const auto nnested = nel.getDoc();
                int nniters = 0;
                bool nnbrk = false;
                for (auto const &nnel : nnested) {
                  switch (nniters) {
                    case 0: {
                      TS_ASSERT_EQUALS(nnel.type(), pot::bson::Element::String);
                      TS_ASSERT(nnel.nameEquals("a"));
                      TS_ASSERT(nnel.strEquals("b"));
                      break;
                    }
                    default: {
                      nnbrk = true;
                      break;
                    }
                  }

                  nniters++;
                  if (nnbrk) {
                    break;
                  }
                }

                TS_ASSERT_EQUALS(nniters, 1);
                break;
              }
              case 3: {
                TS_ASSERT_EQUALS(nel.type(), pot::bson::Element::Binary);
                TS_ASSERT(nel.nameEquals("3"));
                TS_ASSERT_EQUALS(nel.getIndex(), 3);

                uint8_t expected[] = { 4, 3, 2, 1 };
                uint8_t nbuf[sizeof(expected)];
                TS_ASSERT_EQUALS(nel.getBin(nbuf, sizeof(expected)),
                                 sizeof(expected));
                TS_ASSERT_SAME_DATA(nbuf, expected, sizeof(expected));
                break;
              }
              case 4: {
                TS_ASSERT_EQUALS(nel.type(), pot::bson::Element::Boolean);
                TS_ASSERT(nel.nameEquals("4"));
                TS_ASSERT_EQUALS(nel.getIndex(), 4);
                TS_ASSERT_EQUALS(nel.getBool(), true);
                break;
              }
              case 5: {
                TS_ASSERT_EQUALS(nel.type(), pot::bson::Element::Boolean);
                TS_ASSERT(nel.nameEquals("5"));
                TS_ASSERT_EQUALS(nel.getIndex(), 5);
                TS_ASSERT_EQUALS(nel.getBool(), false);
                break;
              }
              case 6: {
                TS_ASSERT_EQUALS(nel.type(), pot::bson::Element::Null);
                TS_ASSERT(nel.nameEquals("6"));
                TS_ASSERT_EQUALS(nel.getIndex(), 6);
                break;
              }
              case 7: {
                TS_ASSERT_EQUALS(nel.type(), pot::bson::Element::Int32);
                TS_ASSERT(nel.nameEquals("7"));
                TS_ASSERT_EQUALS(nel.getIndex(), 7);
                TS_ASSERT_EQUALS(nel.getInt32(), 21);
                break;
              }
              case 8: {
                TS_ASSERT_EQUALS(nel.type(), pot::bson::Element::Int64);
                TS_ASSERT(nel.nameEquals("8"));
                TS_ASSERT_EQUALS(nel.getIndex(), 8);
                TS_ASSERT_EQUALS(nel.getInt32(), 91);
                break;
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

          TS_ASSERT_EQUALS(niters, 9);
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

    TS_ASSERT_EQUALS(iters, 8);
  }
};
