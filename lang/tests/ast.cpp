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
  std::string_view sv = "record sigil { name: []u8; age: u64; }";
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
  std::string_view sv = "enum sigil : u32 { name; age; ssn; phone = 42; }";
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

  std::cout << ast;
}
