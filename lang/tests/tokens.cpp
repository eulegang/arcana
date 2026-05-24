#include <sigil.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "arcana.h"
#include "utils.h"

#define token(T) arcana::Token::T

TEST(lexing, namespace_sample) {
  std::string_view sv = "sigil :: namespace {}";
  sigil::Tokens<arcana::Token> tokens(sv, arcana::tokenizer);

  EXPECT_THAT(testing::sigil_slices(tokens),
              ::testing::ElementsAre("sigil", "::", "namespace", "{", "}"));

  EXPECT_THAT(testing::sigil_token_types(tokens),
              ::testing::ElementsAre(token(ident), token(dcolon), token(ns),
                                     token(lbrace), token(rbrace)));
}

TEST(lexing, struct_sample) {
  std::string_view sv = "slice :: struct { base: *u8, len: u64, }";
  sigil::Tokens<arcana::Token> tokens(sv, arcana::tokenizer);
  EXPECT_THAT(testing::sigil_slices(tokens),
              ::testing::ElementsAre("slice", "::", "struct", "{", "base", ":",
                                     "*", "u8", ",", "len", ":", "u64", ",",
                                     "}"));

  EXPECT_THAT(testing::sigil_token_types(tokens),
              ::testing::ElementsAre(token(ident), token(dcolon), token(strukt),
                                     token(lbrace), token(ident), token(colon),
                                     token(mult), token(ident), token(comma),
                                     token(ident), token(colon), token(ident),
                                     token(comma), token(rbrace)));
}

TEST(lexing, enum_sample) {
  std::string_view sv = "type :: enum { NORMAL, ABNORMAL }";
  sigil::Tokens<arcana::Token> tokens(sv, arcana::tokenizer);
  EXPECT_THAT(testing::sigil_slices(tokens),
              ::testing::ElementsAre("type", "::", "enum", "{", "NORMAL", ",",
                                     "ABNORMAL", "}"));

  EXPECT_THAT(testing::sigil_token_types(tokens),
              ::testing::ElementsAre(token(ident), token(dcolon),
                                     token(enumeration), token(lbrace),
                                     token(ident), token(comma), token(ident),
                                     token(rbrace)));
}

TEST(lexing, bitset_sample) {
  std::string_view sv = "color :: bitset { ERR, ASYNC }";
  sigil::Tokens<arcana::Token> tokens(sv, arcana::tokenizer);
  EXPECT_THAT(testing::sigil_slices(tokens),
              ::testing::ElementsAre("color", "::", "bitset", "{", "ERR", ",",
                                     "ASYNC", "}"));

  EXPECT_THAT(testing::sigil_token_types(tokens),
              ::testing::ElementsAre(token(ident), token(dcolon), token(bitset),
                                     token(lbrace), token(ident), token(comma),
                                     token(ident), token(rbrace)));
}

TEST(lexing, func_sample) {
  std::string_view sv = "do_thing :: (id: u32, premium: bool) -> err!*u32";
  sigil::Tokens<arcana::Token> tokens(sv, arcana::tokenizer);
  EXPECT_THAT(testing::sigil_slices(tokens),
              ::testing::ElementsAre("do_thing", "::", "(", "id", ":", "u32",
                                     ",", "premium", ":", "bool", ")", "->",
                                     "err", "!", "*", "u32"));

  EXPECT_THAT(testing::sigil_token_types(tokens),
              ::testing::ElementsAre(
                  token(ident), token(dcolon), token(lparen), token(ident),
                  token(colon), token(ident), token(comma), token(ident),
                  token(colon), token(ident), token(rparen), token(arrow),
                  token(ident), token(bang), token(mult), token(ident)));
}

TEST(lexing, const_sample) {
  std::string_view sv = "let name: u32 = 42";
  sigil::Tokens<arcana::Token> tokens(sv, arcana::tokenizer);
  EXPECT_THAT(testing::sigil_slices(tokens),
              ::testing::ElementsAre("let", "name", ":", "u32", "=", "42"));

  EXPECT_THAT(testing::sigil_token_types(tokens),
              ::testing::ElementsAre(token(let), token(ident), token(colon),
                                     token(ident), token(assign),
                                     token(integer)));
}

TEST(lexing, var_sample) {
  std::string_view sv = "var name: u32 = 42";
  sigil::Tokens<arcana::Token> tokens(sv, arcana::tokenizer);
  EXPECT_THAT(testing::sigil_slices(tokens),
              ::testing::ElementsAre("var", "name", ":", "u32", "=", "42"));

  EXPECT_THAT(testing::sigil_token_types(tokens),
              ::testing::ElementsAre(token(var), token(ident), token(colon),
                                     token(ident), token(assign),
                                     token(integer)));
}

TEST(lexing, bool_operators) {
  std::string_view sv = "! && || &&= ||=";
  sigil::Tokens<arcana::Token> tokens(sv, arcana::tokenizer);
  EXPECT_THAT(testing::sigil_slices(tokens),
              ::testing::ElementsAre("!", "&&", "||", "&&=", "||="));

  EXPECT_THAT(testing::sigil_token_types(tokens),
              ::testing::ElementsAre(token(bang), token(bool_and),
                                     token(bool_or), token(bool_and_assign),
                                     token(bool_or_assign)));
}

TEST(lexing, num_operators) {
  std::string_view sv = "+ * - / % += *= -= /= %=";
  sigil::Tokens<arcana::Token> tokens(sv, arcana::tokenizer);
  EXPECT_THAT(testing::sigil_slices(tokens),
              ::testing::ElementsAre("+", "*", "-", "/", "%",
                                     "+=", "*=", "-=", "/=", "%="));

  EXPECT_THAT(testing::sigil_token_types(tokens),
              ::testing::ElementsAre(token(plus), token(mult), token(minus),
                                     token(div), token(mod), token(plus_assign),
                                     token(mult_assign), token(minus_assign),
                                     token(div_assign), token(mod_assign)));
}

TEST(lexing, literals) {
  std::string_view sv = "true false 0 32 \"hello\"";
  sigil::Tokens<arcana::Token> tokens(sv, arcana::tokenizer);
  EXPECT_THAT(testing::sigil_slices(tokens),
              ::testing::ElementsAre("true", "false", "0", "32", "\"hello\""));

  EXPECT_THAT(testing::sigil_token_types(tokens),
              ::testing::ElementsAre(token(bool_t), token(bool_f),
                                     token(integer), token(integer),
                                     token(str)));
}

TEST(lexing, line_comments) {
  std::string_view sv = "1 // 0\n 2";
  sigil::Tokens<arcana::Token> tokens(sv, arcana::tokenizer);
  EXPECT_THAT(testing::sigil_slices(tokens), ::testing::ElementsAre("1", "2"));

  EXPECT_THAT(testing::sigil_token_types(tokens),
              ::testing::ElementsAre(token(integer), token(integer)));
}

TEST(lexing, block_comments) {
  std::string_view sv = "1 /* 0 */ 2";
  sigil::Tokens<arcana::Token> tokens(sv, arcana::tokenizer);
  EXPECT_THAT(testing::sigil_slices(tokens), ::testing::ElementsAre("1", "2"));

  EXPECT_THAT(testing::sigil_token_types(tokens),
              ::testing::ElementsAre(token(integer), token(integer)));
}
