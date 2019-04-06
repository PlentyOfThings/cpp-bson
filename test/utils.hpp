#ifndef POT_BSON_TEST_UTILS_H_
#define POT_BSON_TEST_UTILS_H_

void clear_buf(uint8_t buf[], size_t start, size_t end) {
  for (size_t i = start; i < end; i++) {
    buf[i] = 0;
  }
}

void clear_buf(uint8_t buf[], size_t len) {
  clear_buf(buf, 0, len);
}

#endif
