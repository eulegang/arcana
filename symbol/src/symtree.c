#include "symtree.h"
#include <sys/mman.h>
#include <unistd.h>

struct symbol_tree {
  size_t total;
  uint16_t next;
  uint16_t pad[13];
};

typedef struct symbol_node_entry {
  uint16_t next;
  uint16_t prev;
} symbol_node_entry;

struct symbol_node {
  uint16_t next;
  uint16_t tag;
  symbol_node_entry entries[15];
};

struct symbol_tree_curosr {};

symbol_tree *symbol_tree_init(size_t pages) {
  size_t len = pages * getpagesize();

  symbol_tree *self =
      mmap(0, len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);

  if (self == MAP_FAILED) {
    return NULL;
  }

  self->total = len;
  self->next = 0;

  return self;
}

void symbol_tree_deinit(symbol_tree *self) { munmap(self, self->total); }

symbol_tree_cursor symbol_tree_cursor_init(symbol_tree *tree) {
  return (symbol_tree_cursor){.cur = (void *)(tree + 1)};
}

void symbol_tree_cursor_add_symbol(symbol_tree_cursor *cursor, symbol sym) {}
bool symbol_tree_cursor_has_symbol(symbol_tree_cursor *cursor, symbol sym) {}
bool symbol_tree_cursor_enter_symbol(symbol_tree_cursor *cursor, symbol sym) {}
