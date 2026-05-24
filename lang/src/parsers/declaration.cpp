
#include "../arcana.h"
#include "parsers.h"
#include <cstdint>
#include <sigil.h>

namespace arcana {
sigil_state parse_declaration(sigil_state state) {
  state = parse_ident(state);
  if (state.status)
    return state;

  uint16_t ident_node = state.subroot;

  sigil_token token = sigil_state_token(state);
  if (token.type != token_code(dcolon)) {
    state.status |= 4;
    return state;
  }

  sigil_state_next(&state);
  token = sigil_state_token(state);

  uint16_t idx = sigil_state_alloc_node(&state);
  sigil_node *node = sigil_state_node(state, idx);
  node->type = node_code(declare);
  node->child = ident_node;

  sigil_node *ident = sigil_state_node(state, ident_node);

  sigil_state begin = state;
  bool scanning = true;
  while (scanning) {
    token = sigil_state_token(state);
    switch ((Token)token.type) {
    case Token::bitset:
      state = parse_bitset(begin);
      ident->next = state.subroot;
      scanning = false;
      break;

    case Token::ns:
      state = parse_namespace(begin);
      ident->next = state.subroot;
      scanning = false;
      break;

    case Token::enumeration:
      state = parse_enum(begin);
      ident->next = state.subroot;
      scanning = false;
      break;

    case Token::strukt:
      state = parse_struct(begin);
      ident->next = state.subroot;
      scanning = false;
      break;

    case Token::alias:
      state = parse_alias(begin);
      ident->next = state.subroot;
      scanning = false;
      break;

    case Token::opaque:
      sigil_state_next(&state);
      break;

    default:
      state.status |= 4;
      return state;
    }
  }

  if (state.status) {
    return state;
  }

  state.subroot = idx;
  return state;
}
} // namespace arcana
