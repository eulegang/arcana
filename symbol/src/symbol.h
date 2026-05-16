#ifndef __SYMBOL_H
#define __SYMBOL_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
#include <memory>
extern "C" {
#endif

#define SYMBOL_INTERN_FAIL 0xFFFF

typedef struct symbol_table symbol_table;
typedef uint16_t symbol;

symbol_table *symbol_table_init(size_t cap, size_t pages);
void symbol_table_deinit(symbol_table *);

symbol symbol_table_intern(symbol_table *, const char *);
symbol symbol_table_intern_slice(symbol_table *, const char *, size_t);
const char *symbol_table_resolve(symbol_table *, symbol);
uint16_t symbol_table_len(symbol_table *);

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

  const char *resolve(symbol sym) {
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

#endif
#endif
