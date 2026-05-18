#include "symbol.h"
#include <cstdint>
#include <gtest/gtest.h>

TEST(symbol_table, basic) {
  symbol_table *table = symbol_table_init(32, 16);

  uint16_t hello_symbol = symbol_table_intern(table, "Hello");
  uint16_t world_symbol = symbol_table_intern(table, "World");

  EXPECT_EQ(hello_symbol, 0);
  EXPECT_EQ(world_symbol, 1);

  const char *msg_a = symbol_table_resolve(table, hello_symbol);
  const char *msg_b = symbol_table_resolve(table, world_symbol);

  EXPECT_STREQ(msg_a, "Hello");
  EXPECT_STREQ(msg_b, "World");

  symbol_table_deinit(table);
}

TEST(symbol_table, overflow_cap) {
  symbol_table *table = symbol_table_init(4, 16);

  uint16_t hello_symbol = symbol_table_intern(table, "Hello");
  uint16_t world_symbol = symbol_table_intern(table, "World");
  uint16_t hey_symbol = symbol_table_intern(table, "Hey");
  uint16_t there_symbol = symbol_table_intern(table, "There");
  uint16_t sayler_symbol = symbol_table_intern(table, "Sayler");

  EXPECT_EQ(hello_symbol, 0);
  EXPECT_EQ(world_symbol, 1);
  EXPECT_EQ(hey_symbol, 2);
  EXPECT_EQ(there_symbol, 3);
  EXPECT_EQ(sayler_symbol, 0xFFFF);

  EXPECT_STREQ(symbol_table_resolve(table, hello_symbol), "Hello");
  EXPECT_STREQ(symbol_table_resolve(table, world_symbol), "World");
  EXPECT_STREQ(symbol_table_resolve(table, hey_symbol), "Hey");
  EXPECT_STREQ(symbol_table_resolve(table, there_symbol), "There");
  EXPECT_EQ(symbol_table_resolve(table, sayler_symbol), nullptr);

  symbol_table_deinit(table);
}

TEST(symbol_table, prefix_confusion) {
  symbol_table *table = symbol_table_init(32, 16);

  uint16_t prefix_symbol = symbol_table_intern(table, "token_error");
  uint16_t full_symbol = symbol_table_intern(table, "token");

  EXPECT_EQ(prefix_symbol, 0);
  EXPECT_EQ(full_symbol, 1);

  const char *msg_a = symbol_table_resolve(table, prefix_symbol);
  const char *msg_b = symbol_table_resolve(table, full_symbol);

  EXPECT_STREQ(msg_a, "token_error");
  EXPECT_STREQ(msg_b, "token");

  symbol_table_deinit(table);
}
