;
#include "parsers.h"
#include <sigil.h>

namespace arcana {
sigil_state parse_struct_field(sigil_state state) {
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
      .type = node_code(st_field),
  };

  sigil_state_next(&state);
  if (state.status) {
    return state;
  }

  token = sigil_state_token(state);
  if (token.type != token_code(colon)) {
    state.subroot = node;
    return state;
  }

  sigil_state_next(&state);
  if (state.status) {
    return state;
  }

  state = parse_type(state);

  if (state.status)
    return state;

  root->child = state.subroot;

  state.subroot = node;
  return state;
}

sigil_state parse_struct(sigil_state state) {
  uint16_t meta = 0;
  if (sigil_state_token(state).type == token_code(opaque)) {
    meta |= ACCESS_OPAQUE;
    sigil_state_next(&state);
  }

  check_token(strukt);

  data_id struct_meta_id = sigil_state_malloc(&state, sizeof(uint16_t));
  uint16_t *data = (uint16_t *)sigil_state_data(state, struct_meta_id);
  *data = meta;

  uint16_t node = sigil_state_alloc_node(&state);
  sigil_node *root = sigil_state_node(state, node);
  *root = {
      .child = 0,
      .next = 0,
      .offset = struct_meta_id,
      .type = node_code(st),
  };

  uint16_t fields_id = sigil_state_alloc_node(&state);
  sigil_node *fields = sigil_state_node(state, fields_id);
  root->child = fields_id;
  *fields = {
      .child = 0,
      .next = 0,
      .offset = 0xFFFF,
      .type = node_code(st_fields),
  };

  uint16_t decls_id = sigil_state_alloc_node(&state);
  sigil_node *decls = sigil_state_node(state, decls_id);
  fields->next = decls_id;
  *decls = {
      .child = 0,
      .next = 0,
      .offset = 0xFFFF,
      .type = node_code(decls),
  };

  sigil_state_next(&state);
  if (state.status)
    return state;

  sigil_token token = sigil_state_token(state);
  if (token.type != token_code(lbrace)) {
    state.status |= 1;
    return state;
  }

  sigil_state_next(&state);

  sigil_node *fields_cur = fields;
  sigil_node *decls_cur = decls;

  while (true) {
    token = sigil_state_token(state);
    if (token.type == token_code(rbrace)) {
      sigil_state_next(&state);
      break;
    }

    token = sigil_state_token(state);
    sigil_token peek = sigil_state_peek(state, 1);

    if (token.type != token_code(ident)) {
      state.status |= 4;
      return state;
    }

    bool semi_required = false;
    switch ((Token)peek.type) {
    case Token::colon:
      state = parse_struct_field(state);

      if (fields_cur == fields) {
        fields_cur->child = state.subroot;
        fields_cur = sigil_ast_nodes(state.ast) + fields_cur->child;
      } else {
        fields_cur->next = state.subroot;
        fields_cur = sigil_ast_nodes(state.ast) + fields_cur->next;
      }

      semi_required = true;

      break;

    case Token::dcolon:
      state = parse_declaration(state);
      if (decls_cur == decls) {
        decls_cur->child = state.subroot;
        decls_cur = sigil_ast_nodes(state.ast) + decls_cur->child;
      } else {
        decls_cur->next = state.subroot;
        decls_cur = sigil_ast_nodes(state.ast) + decls_cur->next;
      }

      semi_required = false;

      break;

    default:
      state.status |= 4;
      return state;
    }

    if (state.status) {
      return state;
    }

    token = sigil_state_token(state);
    if (token.type == token_code(rbrace)) {
      sigil_state_next(&state);
      break;
    }

    if (semi_required) {
      if (token.type != token_code(semi)) {
        state.status |= 4;
        return state;
      }

      sigil_state_next(&state);
    }
  }

  state.subroot = node;

  return state;
}

sigil_state parse_alias(sigil_state state) {
  sigil_token token = sigil_state_token(state);

  if (token.type != token_code(alias)) {
    state.status |= 4;
    return state;
  }
  uint16_t node = sigil_state_alloc_node(&state);
  sigil_node *root = sigil_state_node(state, node);
  *root = {
      .child = 0,
      .next = 0,
      .offset = 0xFFFF,
      .type = node_code(alias),
  };

  sigil_state_next(&state);

  state = parse_type(state);
  if (state.status)
    return state;

  token = sigil_state_token(state);
  if (token.type != token_code(semi)) {
    state.status |= 4;
    return state;
  }

  sigil_state_next(&state);

  root->child = state.subroot;

  state.subroot = node;
  return state;
}

} // namespace arcana
