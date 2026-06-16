
#include "../arcana.h"
#include "parsers.h"
#include <cstdint>
#include <iostream>
#include <sigil.h>

namespace arcana {
sigil_state parse_declaration(sigil_state state) {
  sigil_state begin = state;
  sigil_token token = sigil_state_token(state);

  switch ((Token)token.type) {
  case Token::module:
    state = parse_module(begin);
    break;

  case Token::record:
    state = parse_struct(begin);
    break;

  case Token::enumeration:
    state = parse_enum(begin);
    break;

  case Token::bitset:
    state = parse_bitset(begin);
    break;

  case Token::alias:
    state = parse_alias(begin);
    break;

  case Token::func:
    state = parse_func(begin);
    break;

  case Token::foreign:
    state = parse_foreign(begin);
    break;

  case Token::ident:
    state = parse_binding(begin);
    check_token(semi);
    next_token();
    break;

  default:
    state.status |= 4;
    return state;
  }

  return state;
}
} // namespace arcana
