#include "parsers.h"
#include <sigil.h>

namespace arcana {
sigil_state parse_enum_backing(sigil_state state) {
  sigil_token token = sigil_state_token(state);

  if (token.type != token_code(lparen)) {
    uint16_t node = sigil_state_alloc_node(&state);
    *sigil_state_node(state, node) = {
        .child = 0,
        .next = 0,
        .offset = 0xFFFF,
        .type = node_code(infer_type),
    };

    state.subroot = node;
    return state;
  }

  sigil_state_next(&state);

  state = parse_ident(state);
  if (state.status) {
    return state;
  }

  token = sigil_state_token(state);

  if (token.type != token_code(rparen)) {
    state.status |= 4;
    return state;
  }

  sigil_state_next(&state);

  return state;
}

sigil_state parse_enum_case(sigil_state state) {
  sigil_token token = sigil_state_token(state);

  if (token.type != token_code(ident)) {
    state.status |= 4;
    return state;
  }

  uint16_t data = sigil_state_malloc(&state, sizeof(uint16_t));
  *(uint16_t *)sigil_state_data(state, data) = state.token_cursor;

  uint16_t node = sigil_state_alloc_node(&state);
  sigil_node *root = sigil_state_node(state, node);
  *root = {
      .child = 0,
      .next = 0,
      .offset = data,
      .type = node_code(en_case),
  };

  sigil_state_next(&state);
  if (state.status) {
    return state;
  }

  token = sigil_state_token(state);
  if (token.type != token_code(assign)) {
    state.subroot = node;
    return state;
  }

  sigil_state_next(&state);
  if (state.status) {
    return state;
  }

  state = sigil_parser_parse_expr(arcana::parser, state, (size_t)Perc::LOWEST);

  root->child = state.subroot;

  state.subroot = node;
  return state;
}

sigil_state parse_enum(sigil_state state) {

  sigil_token token = sigil_state_token(state);

  if (token.type != token_code(enumeration)) {
    state.status |= 4;
    return state;
  }

  uint16_t node = sigil_state_alloc_node(&state);
  sigil_node *root = sigil_state_node(state, node);
  *root = {
      .child = 0,
      .next = 0,
      .offset = 0xFFFF,
      .type = node_code(en),
  };
  sigil_state_next(&state);
  if (state.status)
    return state;

  state = parse_enum_backing(state);

  root->child = state.subroot;

  token = sigil_state_token(state);
  if (token.type != token_code(lbrace)) {
    state.status |= 1;
    return state;
  }

  sigil_state_next(&state);
  if (state.status)
    return state;

  sigil_node *cur = sigil_ast_nodes(state.ast) + root->child;

  while (true) {
    token = sigil_state_token(state);
    if (token.type == token_code(rbrace)) {
      sigil_state_next(&state);
      break;
    }

    state = parse_enum_case(state);
    if (state.status) {
      return state;
    }

    token = sigil_state_token(state);
    if (token.type == token_code(rbrace)) {
      sigil_state_next(&state);
      break;
    }

    cur->next = state.subroot;
    cur = sigil_ast_nodes(state.ast) + cur->next;

    if (token.type != token_code(semi)) {
      state.status |= 4;
      return state;
    }

    sigil_state_next(&state);
  }

  state.subroot = node;
  return state;
}
} // namespace arcana
