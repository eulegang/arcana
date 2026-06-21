#include "symbol.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define MARKER_SCOPE 0x0001
#define MARKER_SYM 0x0000

#define ENTRIES(data) (struct symbol_entry *)(data + 1)
#define CAP(total)                                                             \
  ((total) - sizeof(struct symbol_tree) / sizeof(struct symbol_entry))

struct symbol_tree {
  size_t total;
  size_t len;
};

struct symbol_entry {
  void *data;
  symbol sym;
  uint16_t flags;
  uint32_t _pad;
};

symbol_tree *symbol_tree_init(size_t pages) {
  size_t len = pages * getpagesize();

  symbol_tree *self =
      mmap(0, len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);

  if (self == MAP_FAILED) {
    return NULL;
  }

  self->total = len;
  self->len = 0;

  return self;
}

void symbol_tree_deinit(symbol_tree *self) {
  if (self)
    munmap(self, self->total);
}

bool symbol_tree_define(symbol_tree *self, symbol sym, void *data) {
  if (self->len > CAP(self->total)) {
    abort();
  }

  struct symbol_entry *base = ENTRIES(self);

  size_t len = self->len;
  while (true) {
    if (__builtin_sub_overflow(len, 1, &len)) {
      break;
    }

    struct symbol_entry entry = base[len];

    if (entry.flags == MARKER_SCOPE) {
      break;
    }

    if (entry.sym == sym) {
      return false;
    }
  }

  struct symbol_entry *entry = ENTRIES(self) + self->len++;

  *entry = (struct symbol_entry){
      data,
      sym,
      MARKER_SYM,
      0,

  };

  return true;
}

bool symbol_tree_check(symbol_tree *self, symbol sym, void **data) {
  size_t len = self->len;
  struct symbol_entry *base = ENTRIES(self);

  while (true) {
    if (__builtin_sub_overflow(len, 1, &len)) {
      return false;
    }

    struct symbol_entry entry = base[len];
    if (entry.flags == MARKER_SYM && entry.sym == sym) {
      *data = entry.data;
      return true;
    }
  }
}

void symbol_tree_push(symbol_tree *self) {
  if (self->len > CAP(self->total)) {
    abort();
  }
  struct symbol_entry *entry = ENTRIES(self) + self->len++;

  *entry = (struct symbol_entry){
      NULL,
      0,
      MARKER_SCOPE,
      0,
  };
}

void symbol_tree_pop(symbol_tree *self) {
  if (self->len > CAP(self->total)) {
    abort();
  }

  struct symbol_entry *base = ENTRIES(self);

  while (true) {
    bool end = base[self->len].flags == MARKER_SCOPE;

    if (__builtin_sub_overflow(self->len, 1, &self->len)) {
      break;
    };

    if (end) {
      break;
    }
  }
}
