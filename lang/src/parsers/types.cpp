
#include "parsers.h"
#include <sigil.h>

namespace arcana {
enum class type_perc : size_t {
  LOWEST,

  HIGHEST,
};

sigil_state parse_slice(sigil_state state) {
  sigil_token token = sigil_state_token(state);
  if (token.type != token_code(lbracket)) {
    state.status |= 4;
    return state;
  }

  sigil_state_next(&state);

  token = sigil_state_token(state);

  uint16_t id = sigil_state_alloc_node(&state);
  sigil_node *root = sigil_state_node(state, id);
  switch ((Token)token.type) {
  case Token::rbracket:
    sigil_state_next(&state);
    *root = {
        .child = 0,
        .next = 0,
        .offset = 0xFFFF,
        .type = node_code(slice),
    };

    break;
  case Token::integer: // eventually should be an expr
  {
    sigil_state_span(state, id, {state.token_cursor, state.token_cursor});

    sigil_state_next(&state);

    token = sigil_state_token(state);
    if (token.type != token_code(rbracket)) {
      state.status |= 4;
      return state;
    }

    sigil_state_next(&state);

    *root = {
        .child = 0,
        .next = 0,
        .offset = 0xFFFF,
        .type = node_code(array),
    };
  } break;

  default:
    state.status |= 4;
    return state;
  }

  state =
      sigil_parser_parse_expr(type_parser, state, (size_t)type_perc::LOWEST);

  root->child = state.subroot;

  state.subroot = id;
  return state;
}

sigil_state parse_pointer(sigil_state state) {
  sigil_token token = sigil_state_token(state);
  if (token.type != token_code(mult)) {
    state.status |= 4;
    return state;
  }

  sigil_state_next(&state);

  uint16_t id = sigil_state_alloc_node(&state);
  sigil_node *root = sigil_state_node(state, id);
  *root = {
      .child = 0,
      .next = 0,
      .offset = 0xFFFF,
      .type = node_code(pointer),

  };

  state =
      sigil_parser_parse_expr(type_parser, state, (size_t)type_perc::LOWEST);

  root->child = state.subroot;

  state.subroot = id;
  return state;
}

sigil_state parse_type(sigil_state state) {
  uint16_t id = sigil_state_alloc_node(&state);
  sigil_node *root = sigil_state_node(state, id);
  *root = {
      .child = 0,
      .next = 0,
      .offset = 0xFFFF,
      .type = node_code(ty),
  };

  state =
      sigil_parser_parse_expr(type_parser, state, (size_t)type_perc::LOWEST);

  root->child = state.subroot;

  state.subroot = id;
  return state;
}

sigil_parser *type_parser;

bool terminal(sigil_token_type type);
sigil_state init(sigil_state);

void init_type_parser(void) {
  type_parser = sigil_parser_init((uint16_t)Token::END + 1, terminal, init);

  sigil_parser_slots(type_parser)[(uint16_t)Token::ident] = {
      .prefix = parse_ident,
      .postfix = 0,
      .infix = 0,
      .perc = 0,
  };

  sigil_parser_slots(type_parser)[(uint16_t)Token::lbracket] = {
      .prefix = parse_slice,
      .postfix = 0,
      .infix = 0,
      .perc = 0,
  };

  sigil_parser_slots(type_parser)[(uint16_t)Token::mult] = {
      .prefix = parse_pointer,
      .postfix = 0,
      .infix = 0,
      .perc = 0,
  };

  sigil_parser_slots(type_parser)[(uint16_t)Token::fn] = {
      .prefix = parse_func_type,
      .postfix = 0,
      .infix = 0,
      .perc = 0,
  };
}

void deinit_type_parser(void) { sigil_parser_deinit(type_parser); }

} // namespace arcana
