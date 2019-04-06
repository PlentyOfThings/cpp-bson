#ifndef POT_BSON_SERIALIZER_RESULT_H_
#define POT_BSON_SERIALIZER_RESULT_H_

#include <stdlib.h>

namespace pot {
namespace bson {
namespace serializer {

enum struct Status {
  /**
   * The operation completed without errors.
   */
  Ok,
  /**
   * The document couldn't be completed because it would have overflowed the buffer.
   * If this is returned, then the buffer is assumed to be in an invalid state,
   * and the document will have to be reconstructed from the beginning.
   */
  BufferOverflow,
};

struct Result {
  /**
   * The resulting status code from the attempted action.
   */
  Status status;
  /**
   * This is the total length of the document. Even if there was a buffer
   * overflow, this will still be accurate, and can be used to reallocate
   * a buffer of the required size if necessary.
   */
  size_t len;
};

} // namespace serializer
} // namespace bson
} // namespace pot

#endif
