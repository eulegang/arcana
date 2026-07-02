#include "arcana/types.h"
#include "symbol.h"
#include <gtest/gtest.h>

using namespace arcana::types;

#define EXPECT_SHOVE(subject, output)                                          \
  {                                                                            \
    std::stringstream out;                                                     \
    out << subject;                                                            \
    EXPECT_EQ(out.str(), output);                                              \
  }

TEST(repr, type_id) {
  EXPECT_SHOVE(type_id(type_id::cat::meta, 0), "(meta)0");
  EXPECT_SHOVE(type_id(type_id::cat::bs, 0), "(bs)0");
  EXPECT_SHOVE(type_id(type_id::cat::en, 0), "(en)0");
  EXPECT_SHOVE(type_id(type_id::cat::st, 0), "(st)0");
  EXPECT_SHOVE(type_id(type_id::cat::prim, 0), "(prim)0");
  EXPECT_SHOVE(type_id(type_id::cat::derive, 0), "(derive)0");
  EXPECT_SHOVE(type_id(type_id::cat::fn, 0), "(fn)0");
  EXPECT_SHOVE(type_id(type_id::cat::alias, 0), "(alias)0");
}

TEST(repr, typebase_repr) {

  SymbolTable table(4096, 8);
  table.intern("xyz");
  table.intern("abc");

  Typebase base(table);

  EXPECT_SHOVE(base.repr(type_id()), "null");
  EXPECT_SHOVE(base.repr(type_id(type_id::cat::meta, 1)), "poison");
  EXPECT_SHOVE(base.repr(type_id(type_id::cat::meta, 32)), "!!!");
}
