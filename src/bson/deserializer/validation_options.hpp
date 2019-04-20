#ifndef POT_BSON_DESERIALIZER_VALIDATION_OPTIONS_H_
#define POT_BSON_DESERIALIZER_VALIDATION_OPTIONS_H_

namespace pot {
namespace bson {
namespace deserializer {

struct ValidationOptions {
  bool correct_doc_length;
  bool valid_array_indices;
};

static constexpr ValidationOptions default_validation_options = {
  .correct_doc_length = true,
  .valid_array_indices = true,
};

} // namespace deserializer
} // namespace bson
} // namespace pot

#endif
