#include "symbol.h"
#include <cstdint>
#include <gtest/gtest.h>

TEST(symbol_tree, basic) {
  SymbolTree tree(4);

  tree.define(1, (void *)42);
  tree.define(3, (void *)40);

  void *data;
  ASSERT_TRUE(tree.check(1, &data));
  ASSERT_EQ(data, (void *)42);

  ASSERT_TRUE(tree.check(3, &data));
  ASSERT_EQ(data, (void *)40);

  ASSERT_FALSE(tree.check(5, &data));
}

TEST(symbol_tree, override) {
  SymbolTree tree(4);

  tree.define(1, (void *)0x42);
  tree.define(3, (void *)0x40);
  tree.push();
  tree.define(1, (void *)0x52);

  void *data;
  ASSERT_TRUE(tree.check(1, &data));
  ASSERT_EQ(data, (void *)0x52);

  ASSERT_TRUE(tree.check(3, &data));
  ASSERT_EQ(data, (void *)0x40);

  ASSERT_FALSE(tree.check(5, &data));

  tree.pop();
  ASSERT_TRUE(tree.check(1, &data));
  ASSERT_EQ(data, (void *)0x42);
}

TEST(symbol_tree, failed_override) {
  SymbolTree tree(4);

  ASSERT_TRUE(tree.define(1, (void *)0x42));
  ASSERT_TRUE(tree.define(3, (void *)0x40));
  ASSERT_FALSE(tree.define(1, (void *)0x52));

  void *data;
  ASSERT_TRUE(tree.check(1, &data));
  ASSERT_EQ(data, (void *)0x42);

  ASSERT_TRUE(tree.check(3, &data));
  ASSERT_EQ(data, (void *)0x40);

  ASSERT_FALSE(tree.check(5, &data));
}
