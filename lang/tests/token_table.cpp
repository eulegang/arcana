
#include <sigil.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "arcana.h"

#define token(T) sysl_token::T

#define check(T)                                                               \
  ss.str("");                                                                  \
  ss << arcana::Token::T;                                                      \
  EXPECT_EQ(ss.str(), #T);

TEST(token_table, exact_match) {
  std::stringstream ss;

  check(ident);

  check(ident);
  check(ns);

  ss.str("");
  ss << arcana::Token::strukt;
  EXPECT_EQ(ss.str(), "struct");

  ss.str("");
  ss << arcana::Token::enumeration;
  EXPECT_EQ(ss.str(), "enum");

  check(bitset);
  check(alias);
  check(fn);

  check(let);
  check(var);

  check(integer);
  check(str);

  check(assign);
  check(plus);
  check(comma);
  check(semi);
  check(bang);
  check(minus);
  check(div);
  check(mult);
  check(mod);

  check(plus_assign);
  check(minus_assign);
  check(mult_assign);
  check(div_assign);
  check(mod_assign);

  check(eq);
  check(ne);

  check(lt);
  check(le);
  check(gt);
  check(ge);

  check(lparen);
  check(rparen);
  check(lbrace);
  check(rbrace);
  check(lbracket);
  check(rbracket);

  check(arrow);
  check(dcolon);
  check(colon);

  ss.str("");
  ss << arcana::Token::cond;
  EXPECT_EQ(ss.str(), "if");

  ss.str("");
  ss << arcana::Token::otherwise;
  EXPECT_EQ(ss.str(), "else");

  check(ret);

  check(bool_t);
  check(bool_f);
  check(bool_and);
  check(bool_or);

  check(bool_and_assign);
  check(bool_or_assign);
}
