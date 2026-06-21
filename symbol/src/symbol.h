#ifndef __SYMBOL_H
#define __SYMBOL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
#include <memory>
extern "C" {
#endif

#define SYMBOL_INTERN_FAIL 0xFFFF

typedef struct symbol_table symbol_table;
typedef struct symbol_tree symbol_tree;
typedef uint16_t symbol;

symbol_table *symbol_table_init(size_t cap, size_t pages);
void symbol_table_deinit(symbol_table *);

symbol symbol_table_intern(symbol_table *, const char *);
symbol symbol_table_intern_slice(symbol_table *, const char *, size_t);
const char *symbol_table_resolve(symbol_table *, symbol);
uint16_t symbol_table_len(symbol_table *);

symbol_tree *symbol_tree_init(size_t pages);
void symbol_tree_deinit(symbol_tree *);

bool symbol_tree_define(symbol_tree *, symbol, void *);
bool symbol_tree_check(symbol_tree *, symbol, void **);
void symbol_tree_push(symbol_tree *);
void symbol_tree_pop(symbol_tree *);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
struct SymbolTable {
  using Ptr = std::unique_ptr<symbol_table, decltype(&symbol_table_deinit)>;
  Ptr ptr;

  SymbolTable(size_t cap, size_t pages)
      : ptr{Ptr(symbol_table_init(cap, pages), symbol_table_deinit)} {}

  symbol intern(const char *str) { return symbol_table_intern(ptr.get(), str); }
  symbol intern(std::string_view str) {
    return symbol_table_intern_slice(ptr.get(), str.data(), str.length());
  }

  const char *resolve(symbol sym) const {
    return symbol_table_resolve(ptr.get(), sym);
  }

  struct SymbolIter {
    uint16_t sym;

    uint16_t operator*() const { return sym; }
    SymbolIter operator++() { return {.sym = sym++}; }
    bool operator==(const SymbolIter &other) const { return sym == other.sym; }
  };

  SymbolIter begin() const { return {.sym = 0}; }
  SymbolIter end() const { return {.sym = symbol_table_len(ptr.get())}; }
};

struct SymbolTree {
  using Ptr = std::unique_ptr<symbol_tree, decltype(&symbol_tree_deinit)>;
  Ptr ptr;

  SymbolTree(size_t pages)
      : ptr{Ptr(symbol_tree_init(pages), symbol_tree_deinit)} {}

  bool define(symbol sym, void *data) {
    return symbol_tree_define(ptr.get(), sym, data);
  }

  bool check(symbol sym, void **data) {
    return symbol_tree_check(ptr.get(), sym, data);
  }

  void push() { return symbol_tree_push(ptr.get()); }
  void pop() { symbol_tree_pop(ptr.get()); }
};

#endif
#endif
