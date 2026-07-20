#include <mfile.h>
#include <sigil.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "arcana.h"
#include "arcana/pass.h"
#include "arcana/types.h"
#include "arcana_tokens.h"
#include "symbol.h"

using arcana::types::type_id;

#define load_file(FILE)                                                        \
  mfile file{FILE};                                                            \
  std::string_view sv = file;                                                  \
  arcana::Tokens tokens(sv, arcana::tokenizer);                                \
  arcana::Ast ast{arcana::parser, tokens};                                     \
  SymbolTable syms{4096, 16};                                                  \
  arcana::Diagnostics diagnostics;                                             \
  arcana::pass::NamePass name_pass{tokens, ast, syms, diagnostics};            \
  name_pass.run();                                                             \
                                                                               \
  arcana::types::Typebase base{syms};                                          \
  arcana::types::TypeDefPass type_def{                                         \
      tokens, ast, syms, base, name_pass.overlay, diagnostics};                \
  type_def.run();                                                              \
                                                                               \
  ASSERT_FALSE(diagnostics.has_errors());

#define ASSERT_TYPE(POS, TY)                                                   \
  ASSERT_NE(type_def.overlay.resolve(POS), nullptr);                           \
  EXPECT_EQ(*type_def.overlay.resolve(POS), TY);

TEST(type_pass, func) {
  load_file("func.arc");
  ASSERT_TYPE(0, type_id(type_id::cat::fn, 0));
  ASSERT_TYPE(6, type_id(type_id::cat::derive, 2));
  ASSERT_TYPE(11, type_id(type_id::cat::prim, 6));
}

TEST(type_pass, record) {
  load_file("record.arc");
  ASSERT_TYPE(0, type_id(type_id::cat::st, 0));
  ASSERT_TYPE(6, type_id(type_id::cat::derive, 0));
  ASSERT_TYPE(11, type_id(type_id::cat::prim, 8));
}

TEST(type_pass, bitset) {
  load_file("bitset.arc");
  ASSERT_TYPE(0, type_id(type_id::cat::bs, 0));
}

TEST(type_pass, enum) {
  load_file("enum.arc");
  ASSERT_TYPE(0, type_id(type_id::cat::en, 0));
}

TEST(type_pass, const) {
  load_file("const.arc");

  ASSERT_TYPE(0, type_id(type_id::cat::derive, 0));
  ASSERT_TYPE(2, type_id(type_id::cat::derive, 0));
  ASSERT_TYPE(3, type_id(type_id::cat::derive, 0));
  ASSERT_TYPE(4, type_id(type_id::cat::derive, 0));

  ASSERT_TYPE(5, type_id(type_id::cat::prim, 7));
  ASSERT_TYPE(6, type_id(type_id::cat::prim, 7));
  ASSERT_TYPE(7, type_id(type_id::cat::prim, 7));
  ASSERT_TYPE(9, type_id(type_id::cat::prim, 7));
}

TEST(type_pass, hello) {
  load_file("hello.arc");
  ASSERT_TYPE(0, type_id(type_id::cat::en, 0));
}
