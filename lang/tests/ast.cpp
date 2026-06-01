#include <mfile.h>
#include <sigil.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "arcana.h"
#include "arcana_tokens.h"
#include "utils.h"

TEST(parsing, mod) {
  std::string_view sv = "module sigil {}";
  arcana::Tokens tokens(sv, arcana::tokenizer);
  arcana::Ast ast{arcana::parser, tokens};

  ASSERT_EQ(ast.node_count(), 3);

  EXPECT_EQ((arcana::Node)ast[1].type, arcana::Node::ns);
  EXPECT_EQ((arcana::Node)ast[2].type, arcana::Node::ident);
}

TEST(parsing, record) {
  mfile file{"corpus/record.arc"};
  std::string_view sv = file;
  arcana::Tokens tokens(sv, arcana::tokenizer);
  arcana::Ast ast{arcana::parser, tokens};

  ASSERT_EQ(ast.node_count(), 13);

  uint16_t id = 1;
  EXPECT_EQ(ast[id++].type, arcana::Node::st);
  EXPECT_EQ(ast[id++].type, arcana::Node::ident);
  EXPECT_EQ(ast[id++].type, arcana::Node::st_fields);
  EXPECT_EQ(ast[id++].type, arcana::Node::st_field);
  EXPECT_EQ(ast[id++].type, arcana::Node::ident);
  EXPECT_EQ(ast[id++].type, arcana::Node::ty);
  EXPECT_EQ(ast[id++].type, arcana::Node::slice);
  EXPECT_EQ(ast[id++].type, arcana::Node::ident);
  EXPECT_EQ(ast[id++].type, arcana::Node::st_field);
  EXPECT_EQ(ast[id++].type, arcana::Node::ident);
  EXPECT_EQ(ast[id++].type, arcana::Node::ty);
  EXPECT_EQ(ast[id++].type, arcana::Node::ident);
}

TEST(parsing, enumeration) {
  mfile file{"corpus/enum.arc"};
  std::string_view sv = file;
  arcana::Tokens tokens(sv, arcana::tokenizer);
  arcana::Ast ast{arcana::parser, tokens};

  ASSERT_EQ(ast.node_count(), 13);

  uint16_t id = 1;
  EXPECT_EQ(ast[id++].type, arcana::Node::en);
  EXPECT_EQ(ast[id++].type, arcana::Node::ident);
  EXPECT_EQ(ast[id++].type, arcana::Node::ident);
  EXPECT_EQ(ast[id++].type, arcana::Node::en_case);
  EXPECT_EQ(ast[id++].type, arcana::Node::ident);
  EXPECT_EQ(ast[id++].type, arcana::Node::en_case);
  EXPECT_EQ(ast[id++].type, arcana::Node::ident);
  EXPECT_EQ(ast[id++].type, arcana::Node::en_case);
  EXPECT_EQ(ast[id++].type, arcana::Node::ident);
  EXPECT_EQ(ast[id++].type, arcana::Node::en_case);
  EXPECT_EQ(ast[id++].type, arcana::Node::ident);
}

TEST(parsing, bitset) {
  mfile file{"corpus/bitset.arc"};
  std::string_view sv = file;
  arcana::Tokens tokens(sv, arcana::tokenizer);
  arcana::Ast ast{arcana::parser, tokens};

  ASSERT_EQ(ast.node_count(), 14);

  uint16_t id = 1;
  EXPECT_EQ(ast[id++].type, arcana::Node::bs);
  EXPECT_EQ(ast[id++].type, arcana::Node::ident);
  EXPECT_EQ(ast[id++].type, arcana::Node::ident);
  EXPECT_EQ(ast[id++].type, arcana::Node::bs_case);
  EXPECT_EQ(ast[id++].type, arcana::Node::ident);
  EXPECT_EQ(ast[id++].type, arcana::Node::bs_case);
  EXPECT_EQ(ast[id++].type, arcana::Node::ident);
  EXPECT_EQ(ast[id++].type, arcana::Node::bs_case);
  EXPECT_EQ(ast[id++].type, arcana::Node::ident);
  EXPECT_EQ(ast[id++].type, arcana::Node::literal);
  EXPECT_EQ(ast[id++].type, arcana::Node::bs_case);
  EXPECT_EQ(ast[id++].type, arcana::Node::ident);
  EXPECT_EQ(ast[id++].type, arcana::Node::literal);
}

TEST(parsing, alias) {
  std::string_view sv = "alias id = u16;";
  arcana::Tokens tokens(sv, arcana::tokenizer);
  arcana::Ast ast{arcana::parser, tokens};

  ASSERT_EQ(ast.node_count(), 5);

  uint16_t id = 1;
  EXPECT_EQ(ast[id++].type, arcana::Node::alias);
  EXPECT_EQ(ast[id++].type, arcana::Node::ident);
  EXPECT_EQ(ast[id++].type, arcana::Node::ty);
  EXPECT_EQ(ast[id++].type, arcana::Node::ident);
}
