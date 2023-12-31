#ifndef BLAKE3_H
#define BLAKE3_H

#include <stdint.h>

# define memcpy __builtin_memcpy
# define memset __builtin_memset

typedef uint32_t size_t;

int strlen (const char *str) {
    int len = 0;
    while (* str ++) ++ len;
    return len;
}

#define BLAKE3_API

#define BLAKE3_VERSION_STRING "1.5.0"
#define BLAKE3_KEY_LEN 32
#define BLAKE3_OUT_LEN 32
#define BLAKE3_BLOCK_LEN 64
#define BLAKE3_CHUNK_LEN 1024
#define BLAKE3_MAX_DEPTH 54

// This struct is a private implementation detail. It has to be here because
// it's part of blake3_hasher below.
typedef struct {
  uint32_t cv[8];
  uint64_t chunk_counter;
  uint8_t buf[BLAKE3_BLOCK_LEN];
  uint8_t buf_len;
  uint8_t blocks_compressed;
  uint8_t flags;
} blake3_chunk_state;

typedef struct {
  uint32_t key[8];
  blake3_chunk_state chunk;
  uint8_t cv_stack_len;
  // The stack size is MAX_DEPTH + 1 because we do lazy merging. For example,
  // with 7 chunks, we have 3 entries in the stack. Adding an 8th chunk
  // requires a 4th entry, rather than merging everything down to 1, because we
  // don't know whether more input is coming. This is different from how the
  // reference implementation does things.
  uint8_t cv_stack[(BLAKE3_MAX_DEPTH + 1) * BLAKE3_OUT_LEN];
} blake3_hasher;

BLAKE3_API const char *blake3_version(void);
BLAKE3_API void blake3_hasher_init(blake3_hasher *self);
BLAKE3_API void blake3_hasher_init_keyed(blake3_hasher *self,
                                         const uint8_t key[BLAKE3_KEY_LEN]);
BLAKE3_API void blake3_hasher_init_derive_key(blake3_hasher *self, const char *context);
BLAKE3_API void blake3_hasher_init_derive_key_raw(blake3_hasher *self, const void *context,
                                                  size_t context_len);
BLAKE3_API void blake3_hasher_update(blake3_hasher *self, const void *input,
                                     size_t input_len);
BLAKE3_API void blake3_hasher_finalize(const blake3_hasher *self, uint8_t *out,
                                       size_t out_len);
BLAKE3_API void blake3_hasher_finalize_seek(const blake3_hasher *self, uint64_t seek,
                                            uint8_t *out, size_t out_len);
BLAKE3_API void blake3_hasher_reset(blake3_hasher *self);

void __attribute__((export_name("BLAKE3"))) BLAKE3 (const unsigned char* data, unsigned char *hash) {
    blake3_hasher hasher;
    blake3_hasher_init(& hasher);
    blake3_hasher_update(& hasher, data, hash - data);
    blake3_hasher_finalize(& hasher, hash, 32);
}

#endif /* BLAKE3_H */
