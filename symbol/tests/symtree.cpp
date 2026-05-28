#include "symtree.h"
#include <gtest/gtest.h>

TEST(symbol_tree, basic) {
  SymbolTree tree{4};
  SymbolTree::Cursor cursor{tree};

  cursor.add(3);
}
