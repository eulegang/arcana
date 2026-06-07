#include "../arcana.h"
#include <cstdint>
#include <sigil.h>

namespace arcana {
sigil_state parse_lit(sigil_state state) {
  sigil_token token = sigil_state_token(state);

  switch ((arcana::Token)token.type) {
  case Token::integer: {
    uint16_t data = sigil_state_malloc(&state, sizeof(uint16_t));
    *(uint16_t *)sigil_state_data(state, data) = state.token_cursor;
    uint16_t idx = sigil_state_alloc_node(&state);
    *sigil_state_node(state, idx) = {
        .child = 0,
        .next = 0,
        .offset = data,
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

} // namespace arcana
