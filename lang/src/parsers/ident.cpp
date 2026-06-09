#include "../arcana.h"
#include "parsers.h"
#include <cstdint>
#include <sigil.h>

sigil_state arcana::parse_ident(sigil_state state) {
  sigil_token token = sigil_state_token(state);

  if (token.type != token_code(ident)) {
    state.status |= 1;
    return state;
  }

  uint16_t node = sigil_state_alloc_node(&state);

  state.subroot = node;
  *sigil_state_node(state, node) = {
      .child = 0,
      .next = 0,
      .offset = 0xFFFF,
      .type = node_code(ident),
  };

  sigil_state_span(state, node, {state.token_cursor, state.token_cursor});

  sigil_state_next(&state);
  if (state.status) {
    return state;
  }

  return state;
}
