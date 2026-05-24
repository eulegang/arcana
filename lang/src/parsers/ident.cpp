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
  uint16_t data = sigil_state_malloc(&state, sizeof(uint16_t));
  uint16_t *data_ptr = (uint16_t *)sigil_state_data(state, data);

  state.subroot = node;
  *sigil_state_node(state, node) = {
      .child = 0,
      .next = 0,
      .offset = data,
      .type = node_code(ident),
  };

  *data_ptr = state.token_cursor;

  sigil_state_next(&state);
  if (state.status) {
    return state;
  }

  return state;
}
