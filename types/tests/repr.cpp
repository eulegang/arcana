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

TEST(typebase_repr, meta) {
  SymbolTable table(4096, 8);
  Typebase base(table);

  EXPECT_SHOVE(base.repr(type_id::null), "null");
  EXPECT_SHOVE(base.repr(type_id::poison), "poison");
  EXPECT_SHOVE(base.repr(type_id(type_id::cat::meta, 32)), "!!!");
}

TEST(typebase_repr, prims) {
  SymbolTable table(4096, 8);
  Typebase base(table);

  EXPECT_SHOVE(base.repr(type_id(type_id::cat::prim, 0)), "void");
  EXPECT_SHOVE(base.repr(type_id(type_id::cat::prim, 1)), "bool");
  EXPECT_SHOVE(base.repr(type_id(type_id::cat::prim, 2)), "u8");
  EXPECT_SHOVE(base.repr(type_id(type_id::cat::prim, 3)), "i8");
  EXPECT_SHOVE(base.repr(type_id(type_id::cat::prim, 4)), "u16");
  EXPECT_SHOVE(base.repr(type_id(type_id::cat::prim, 5)), "i16");
  EXPECT_SHOVE(base.repr(type_id(type_id::cat::prim, 6)), "u32");
  EXPECT_SHOVE(base.repr(type_id(type_id::cat::prim, 7)), "i32");
  EXPECT_SHOVE(base.repr(type_id(type_id::cat::prim, 8)), "u64");
  EXPECT_SHOVE(base.repr(type_id(type_id::cat::prim, 9)), "i64");
  EXPECT_SHOVE(base.repr(type_id(type_id::cat::prim, 10)), "f32");
  EXPECT_SHOVE(base.repr(type_id(type_id::cat::prim, 11)), "f64");

  EXPECT_DEATH({ base.repr(type_id(type_id::cat::prim, 12)); }, "");
}

TEST(typebase_repr, fns) {
  SymbolTable table(4096, 8);
  Typebase base(table);

  auto [empty_id, empty] = base.generate<Fn>();
  auto [res_id, res] = base.generate<Fn>();
  res.ret = type_id::boolean;

  auto [void_res_id, void_res] = base.generate<Fn>();
  void_res.ret = type_id::unit;

  auto [fallible_bool_id, fallible_bool] = base.generate<Fn>();
  fallible_bool.ret = type_id::boolean;
  fallible_bool.err = type_id::boolean;

  auto [args_id, args] = base.generate<Fn>();
  args.params = {type_id::boolean, type_id(type_id::cat::prim, 8)};

  EXPECT_SHOVE(base.repr(empty_id), "() -> void");
  EXPECT_SHOVE(base.repr(res_id), "() -> bool");
  EXPECT_SHOVE(base.repr(void_res_id), "() -> void");
  EXPECT_SHOVE(base.repr(fallible_bool_id), "() -> bool!bool");
  EXPECT_SHOVE(base.repr(args_id), "(bool, u64) -> void");
}
