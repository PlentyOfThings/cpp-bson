#ifndef POT_BSON_SERIALIZER_RESULT_H_
#define POT_BSON_SERIALIZER_RESULT_H_

namespace pot {
namespace bson {
namespace serializer {

enum struct kResult {
  /**
   * The operation completed without errors.
   */
  Ok,
  /**
   * The requested operation would have overflowed the buffer.
   */
  BufferOverflow,
  /**
   * The document has already been ended, and no more operations are allowed.
   */
  Ended,
};

}
} // namespace bson
} // namespace pot

#endif
