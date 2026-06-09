#include <mfile.h>
#include <sigil.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "arcana.h"
#include "arcana/pass.h"
#include "arcana_tokens.h"
#include "symbol.h"
#include "utils.h"

TEST(symbol, func) {
  mfile file{"corpus/func.arc"};
  std::string_view sv = file;
  arcana::Tokens tokens(sv, arcana::tokenizer);
  arcana::Ast ast{arcana::parser, tokens};
  SymbolTable syms{4096, 16};
  arcana::Diagnostics diagnostics;
  arcana::pass::NamePass name_pass{tokens, ast, syms, diagnostics};
  name_pass.run();

  ASSERT_FALSE(diagnostics);

  EXPECT_EQ(name_pass.overlay.resolve(2)->_parent, 0);
  EXPECT_EQ(name_pass.overlay.resolve(2)->_symbol, syms.intern("main"));

  EXPECT_EQ(name_pass.overlay.resolve(5)->_parent, 0);
  EXPECT_EQ(name_pass.overlay.resolve(5)->_symbol, syms.intern("args"));

  EXPECT_EQ(name_pass.overlay.resolve(9)->_parent, 0);
  EXPECT_EQ(name_pass.overlay.resolve(9)->_symbol, syms.intern("u8"));

  EXPECT_EQ(name_pass.overlay.resolve(12)->_parent, 0);
  EXPECT_EQ(name_pass.overlay.resolve(12)->_symbol, syms.intern("u32"));
}
