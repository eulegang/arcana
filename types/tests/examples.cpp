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

TEST(type_pass, func) {
  mfile file{"func.arc"};
  std::string_view sv = file;
  arcana::Tokens tokens(sv, arcana::tokenizer);
  arcana::Ast ast{arcana::parser, tokens};
  SymbolTable syms{4096, 16};
  arcana::Diagnostics diagnostics;
  arcana::pass::NamePass name_pass{tokens, ast, syms, diagnostics};
  name_pass.run();

  arcana::types::Typebase base{syms};
  arcana::types::TypeDefPass type_def{
      tokens, ast, syms, base, name_pass.overlay, diagnostics};
  type_def.run();

  ASSERT_NE(type_def.overlay.resolve(0), nullptr);
  EXPECT_EQ(*type_def.overlay.resolve(0), type_id(type_id::cat::fn, 0));

  ASSERT_NE(type_def.overlay.resolve(6), nullptr);
  EXPECT_EQ(*type_def.overlay.resolve(6), type_id(type_id::cat::derive, 2));

  ASSERT_NE(type_def.overlay.resolve(11), nullptr);
  EXPECT_EQ(*type_def.overlay.resolve(11), type_id(type_id::cat::prim, 6));
}

TEST(type_pass, record) {
  mfile file{"record.arc"};
  std::string_view sv = file;
  arcana::Tokens tokens(sv, arcana::tokenizer);
  arcana::Ast ast{arcana::parser, tokens};
  arcana::Diagnostics diagnostics;
  SymbolTable syms{4096, 16};
  arcana::pass::NamePass name_pass{tokens, ast, syms, diagnostics};
  name_pass.run();

  arcana::types::Typebase base{syms};
  arcana::types::TypeDefPass type_def{
      tokens, ast, syms, base, name_pass.overlay, diagnostics};
  type_def.run();

  ASSERT_NE(type_def.overlay.resolve(0), nullptr);
  EXPECT_EQ(*type_def.overlay.resolve(0), type_id(type_id::cat::st, 0));

  ASSERT_NE(type_def.overlay.resolve(6), nullptr);
  EXPECT_EQ(*type_def.overlay.resolve(6), type_id(type_id::cat::derive, 0));

  ASSERT_NE(type_def.overlay.resolve(11), nullptr);
  EXPECT_EQ(*type_def.overlay.resolve(11), type_id(type_id::cat::prim, 8));
}

TEST(type_pass, bitset) {
  mfile file{"bitset.arc"};
  std::string_view sv = file;
  arcana::Tokens tokens(sv, arcana::tokenizer);
  arcana::Ast ast{arcana::parser, tokens};
  SymbolTable syms{4096, 16};
  arcana::Diagnostics diagnostics;
  arcana::pass::NamePass name_pass{tokens, ast, syms, diagnostics};
  name_pass.run();

  arcana::types::Typebase base{syms};
  arcana::types::TypeDefPass type_def{
      tokens, ast, syms, base, name_pass.overlay, diagnostics};
  type_def.run();

  ASSERT_NE(type_def.overlay.resolve(0), nullptr);
  EXPECT_EQ(*type_def.overlay.resolve(0), type_id(type_id::cat::bs, 0))
      << *type_def.overlay.resolve(0);
}

TEST(type_pass, enum) {
  mfile file{"enum.arc"};
  std::string_view sv = file;
  arcana::Tokens tokens(sv, arcana::tokenizer);
  arcana::Ast ast{arcana::parser, tokens};
  SymbolTable syms{4096, 16};
  arcana::Diagnostics diagnostics;
  arcana::pass::NamePass name_pass{tokens, ast, syms, diagnostics};
  name_pass.run();

  arcana::types::Typebase base{syms};
  arcana::types::TypeDefPass type_def{
      tokens, ast, syms, base, name_pass.overlay, diagnostics};
  type_def.run();

  ASSERT_NE(type_def.overlay.resolve(0), nullptr);
  EXPECT_EQ(*type_def.overlay.resolve(0), type_id(type_id::cat::en, 0))
      << *type_def.overlay.resolve(0);
}
