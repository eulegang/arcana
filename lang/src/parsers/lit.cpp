#include "../arcana.h"
#include <cstdint>
#include <sigil.h>

namespace arcana {
sigil_state parse_lit(sigil_state state) {
  sigil_token token = sigil_state_token(state);

  Primitive prim = Primitive::integer;
  switch ((arcana::Token)token.type) {
  case Token::integer:
    prim = Primitive::integer;
    break;

  default:
    state.status |= 1;
    return state;
    break;
  }

  uint16_t data = sigil_state_malloc(&state, sizeof(LiteralData));
  *(LiteralData *)sigil_state_data(state, data) = {
      .token = state.token_cursor,
      .prim = prim,
  };

  uint16_t idx = sigil_state_alloc_node(&state);
  *sigil_state_node(state, idx) = {
      .child = 0,
      .next = 0,
      .offset = data,
      .type = (uint16_t)Node::literal,
  };

  sigil_state_next(&state);

  state.subroot = idx;
  return state;
}

} // namespace arcana
