#pragma once

#include "stddef.h"
#include "symbol.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
#include <memory>
extern "C" {
#endif

typedef struct symbol_tree symbol_tree;
typedef struct symbol_tree_cursor {
  void *cur;
} symbol_tree_cursor;

symbol_tree *symbol_tree_init(size_t pages);
void symbol_tree_deinit(symbol_tree *);

symbol_tree_cursor symbol_tree_cursor_init(symbol_tree *);
bool symbol_tree_cursor_has_symbol(symbol_tree_cursor *, symbol sym);
void symbol_tree_cursor_add_symbol(symbol_tree_cursor *, symbol sym);
bool symbol_tree_cursor_enter_symbol(symbol_tree_cursor *, symbol sym);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

struct SymbolTree {
  using Ptr = std::unique_ptr<symbol_tree, decltype(&symbol_tree_deinit)>;
  Ptr ptr;

  SymbolTree(size_t pages)
      : ptr{Ptr(symbol_tree_init(pages), symbol_tree_deinit)} {}

  struct Cursor {
    symbol_tree_cursor cursor;

    Cursor(const SymbolTree &tree)
        : cursor{symbol_tree_cursor_init(tree.ptr.get())} {}
    Cursor(const Cursor &cursor) : cursor{cursor.cursor} {}

    void add(symbol sym) { symbol_tree_cursor_add_symbol(&cursor, sym); }
    bool has(symbol sym) { return symbol_tree_cursor_has_symbol(&cursor, sym); }
    bool enter(symbol sym) {
      return symbol_tree_cursor_enter_symbol(&cursor, sym);
    }
  };
};

#endif
