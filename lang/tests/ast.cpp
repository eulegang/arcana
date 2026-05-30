#include <sigil.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "arcana.h"
#include "arcana_tokens.h"
#include "utils.h"

TEST(parsing, module) {
  std::string_view sv = "module sigil {}";
  arcana::Tokens tokens(sv, arcana::tokenizer);
  arcana::Ast ast{arcana::parser, tokens};

  ASSERT_EQ(ast.node_count(), 3);

  EXPECT_EQ(ast[1].type, arcana::Node::ns);
  EXPECT_EQ(ast[2].type, arcana::Node::ident);

  std::cout << ast;
}
