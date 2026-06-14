
#include "../arcana.h"
#include "parsers.h"
#include <cstdint>
#include <iostream>
#include <sigil.h>

namespace arcana {
sigil_state parse_declaration(sigil_state state) {
  sigil_state begin = state;
  sigil_token token = sigil_state_token(state);

  bool scanning = true;
  while (scanning) {
    switch ((Token)token.type) {
    case Token::module:
      state = parse_module(begin);
      scanning = false;
      break;

    case Token::record:
      state = parse_struct(begin);
      scanning = false;
      break;

    case Token::enumeration:
      state = parse_enum(begin);
      scanning = false;
      break;

    case Token::bitset:
      state = parse_bitset(begin);
      scanning = false;
      break;

    case Token::alias:
      state = parse_alias(begin);
      scanning = false;
      break;

    case Token::func:
      state = parse_func(begin);
      scanning = false;
      break;

    case Token::foreign:
      state = parse_foreign(begin);
      scanning = false;
      break;

    default:
      state.status |= 4;
      return state;
    }
  }

  return state;
}
} // namespace arcana
