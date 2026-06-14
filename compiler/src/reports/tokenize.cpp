#include "../reports.h"
#include <chroma.h>
#include <iostream>

chroma::basic_t token_color(arcana::Token);

void report::tokens(const sigil::Tokens<arcana::Token> &tokens) {

  size_t len = tokens.length();

  for (size_t i = 0; i < len; i++) {
    arcana::Token type = tokens[i].type;

    if (verbose) {
      std::string_view text = tokens.content(i);
      sigil_linemeta meta = tokens.linemeta(i);
      std::cout << chroma::cyan << i << "\t";
      std::cout << token_color(type) << type << "\t";
      std::cout << chroma::yellow << meta.line << ":" << meta.column << "\t";
      std::cout << chroma::purple << text << chroma::clear << std::endl;
    } else {
      std::cout << token_color(type) << type << chroma::clear << std::endl;
    }
  }
}

chroma::basic_t token_color(arcana::Token type) {

  switch (type) {
  case arcana::Token::ident:
    return chroma::cyan;
  case arcana::Token::ns:
  case arcana::Token::strukt:
  case arcana::Token::record:
  case arcana::Token::enumeration:
  case arcana::Token::bitset:
  case arcana::Token::alias:
  case arcana::Token::fn:
  case arcana::Token::func:
  case arcana::Token::opaque:
  case arcana::Token::module:
  case arcana::Token::priv:
  case arcana::Token::foreign:
  case arcana::Token::dot:
    return chroma::purple;

  case arcana::Token::cond:
  case arcana::Token::otherwise:
  case arcana::Token::ret:
  case arcana::Token::let:
  case arcana::Token::var:
  case arcana::Token::mut:
    return chroma::purple;

  case arcana::Token::integer:
  case arcana::Token::str:
  case arcana::Token::bool_t:
  case arcana::Token::bool_f:
    return chroma::cyan;

  case arcana::Token::bool_and:
  case arcana::Token::bool_or:
  case arcana::Token::bool_and_assign:
  case arcana::Token::bool_or_assign:
  case arcana::Token::assign:
  case arcana::Token::plus:
  case arcana::Token::bang:
  case arcana::Token::minus:
  case arcana::Token::div:
  case arcana::Token::mult:
  case arcana::Token::mod:
  case arcana::Token::plus_assign:
  case arcana::Token::minus_assign:
  case arcana::Token::mult_assign:
  case arcana::Token::div_assign:
  case arcana::Token::mod_assign:
  case arcana::Token::eq:
  case arcana::Token::ne:
  case arcana::Token::lt:
  case arcana::Token::le:
  case arcana::Token::gt:
  case arcana::Token::ge:
    return chroma::blue;

  case arcana::Token::comma:
  case arcana::Token::semi:
  case arcana::Token::lparen:
  case arcana::Token::rparen:
  case arcana::Token::lbrace:
  case arcana::Token::rbrace:
  case arcana::Token::lbracket:
  case arcana::Token::rbracket:
  case arcana::Token::arrow:
  case arcana::Token::dcolon:
  case arcana::Token::colon:
    return chroma::yellow;
  }

  return chroma::cyan;
}
