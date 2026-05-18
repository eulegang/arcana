#include "symbol.h"
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

/*
 * as it stands right now the symbols will probably be small.
 *
 * because of this I'm elect to do a scan on intern which is linear time.
 *
 * this could benefit from a prefix list for intern in the future.
 */

struct symbol_table {
  size_t total;
  size_t cap;
  size_t len;
};

symbol_table *symbol_table_init(size_t cap, size_t pages) {
  size_t len = pages * getpagesize();

  symbol_table *self =
      mmap(0, len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);

  if (self == MAP_FAILED) {
    return NULL;
  }

  self->total = len;
  self->cap = cap;
  self->len = 0;

  return self;
}

void symbol_table_deinit(symbol_table *self) {
  if (self)
    munmap(self, self->total);
}

symbol symbol_table_intern_slice(symbol_table *self, const char *content,
                                 size_t len) {
  char *base = (char *)(((uint16_t *)(self + 1)) + self->cap);
  uint16_t *index_base = (uint16_t *)(self + 1);

  for (size_t i = 0; i < self->len; i++) {
    uint16_t off = index_base[i];

    if (memcmp(base + off, content, len + 1) == 0) {
      return i;
    }
  }

  if (self->len >= self->cap) {
    return SYMBOL_INTERN_FAIL;
  }

  symbol result = self->len;
  if (!self->len) {
    memcpy(base, content, len);
    base[len + 1] = 0;
    index_base[0] = 0;
  } else {
    uint16_t prior = index_base[self->len - 1];
    size_t prior_len = strlen(base + prior);
    uint16_t data_offset = prior + prior_len + 1;
    char *dest = base + data_offset;

    memcpy(dest, content, len);
    dest[len + 1] = 0;
    index_base[self->len] = data_offset;
  }

  self->len++;
  return result;
}

symbol symbol_table_intern(symbol_table *self, const char *content) {
  size_t len = strlen(content);
  return symbol_table_intern_slice(self, content, len);
}

const char *symbol_table_resolve(symbol_table *self, symbol sym) {
  if (sym == SYMBOL_INTERN_FAIL)
    return NULL;

  if (sym >= self->len)
    return NULL;

  uint16_t *index_base = (uint16_t *)(self + 1);
  uint16_t offset = index_base[sym];
  const char *base = (const char *)(index_base + self->cap);

  return base + offset;
}

uint16_t symbol_table_len(symbol_table *self) { return self->len; }
