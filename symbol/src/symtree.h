#pragma once

#include "stddef.h"
#include <stdint.h>

#ifdef __cplusplus
#include <memory>
extern "C" {
#endif

typedef struct symbol_tree symbol_tree;

typedef struct {
  uint32_t elems[20];
};

symbol_tree *symbol_tree_init(size_t pages);
void symbol_tree_deinit(symbol_tree *);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

struct SymbolTable {
  using Ptr = std::unique_ptr<symbol_tree, decltype(&symbol_tree_deinit)>;
  Ptr ptr;

  SymbolTable(size_t pages)
      : ptr{Ptr(symbol_tree_init(pages), symbol_tree_deinit)} {}
};

#endif
