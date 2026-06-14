#include "../arcana.h"
#include "../parsers/parsers.h"
#include <cstdint>
#include <sigil.h>

namespace arcana {
sigil_state parse_lit(sigil_state state) {
  sigil_token token = sigil_state_token(state);

  switch ((arcana::Token)token.type) {
  case Token::integer: {
    uint16_t idx = sigil_state_alloc_node(&state);
    sigil_state_span(state, idx, {state.token_cursor, state.token_cursor});
    *sigil_state_node(state, idx) = {
        .child = 0,
        .next = 0,
        .offset = 0xFFFF,
        .type = (uint16_t)Node::integer,
    };

    sigil_state_next(&state);

    state.subroot = idx;
  } break;

  default:
    state.status |= 1;
    return state;
    break;
  }

  return state;
}

sigil_state parse_group(sigil_state state) {
  check_token(lparen);
  next_token();

  state = parse_expr(state);

  check_token(rparen);
  next_token();

  return state;
}

} // namespace arcana
