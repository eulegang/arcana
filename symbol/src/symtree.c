#include "symtree.h"
#include <sys/mman.h>
#include <unistd.h>

struct symbol_tree {
  size_t total;
  size_t pad[7];
};

symbol_tree *symbol_tree_init(size_t pages) {
  size_t len = pages * getpagesize();

  symbol_tree *self =
      mmap(0, len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);

  if (self == MAP_FAILED) {
    return NULL;
  }

  self->total = len;

  return self;
}

void symbol_tree_deinit(symbol_tree *self) { munmap(self, self->total); }
