
#include "arcana.h"
#include "monkey.h"
#include <cstdint>

arcana_parser_state monkey_parse_let(arcana_parser_state state) {
  arcana_parse_node *nodes = arcana_parser_ast_nodes(state.ast);
  void *data = arcana_parser_ast_data(state.ast);

  uint16_t let_node = arcana_parser_alloc_node(&state);
  uint16_t ident_node = arcana_parser_alloc_node(&state);
  uint16_t lit_node = arcana_parser_alloc_node(&state);

  uint16_t ident_addr = arcana_parser_ast_malloc(&state, sizeof(monkey_slice));
  uint16_t lit_addr = arcana_parser_ast_malloc(&state, sizeof(monkey_slice));

  if (state.status) {
    return state;
  }

  nodes[state.last_root_child].next = let_node;

  nodes[let_node] = {
      .child = ident_node,
      .next = 0,
      .offset = 0xFFFF,
      .type = (uint16_t)monkey_node_type::let,
  };

  nodes[ident_node] = {
      .child = 0,
      .next = lit_node,
      .offset = ident_addr,
      .type = (uint16_t)monkey_node_type::ident,
  };

  nodes[lit_node] = {
      .child = 0,
      .next = 0,
      .offset = lit_addr,
      .type = (uint16_t)monkey_node_type::lit,
  };

  state = arcana_parser_expect_token(state, monkey_token_type_let);
  if (state.status) {
    return state;
  }

  arcana_parser_ast_next_token(&state);
  if (state.status) {
    return state;
  }

  state = arcana_parser_expect_token(state, monkey_token_type_ident);
  if (state.status) {
    return state;
  }

  arcana_token token = arcana_tokens_data(state.tokens)[state.token_cursor];
  *(monkey_slice *)((char *)data + ident_addr) = {
      .base = token.off,
      .len = token.len,
  };

  arcana_parser_ast_next_token(&state);
  if (state.status) {
    return state;
  }

  state = arcana_parser_expect_token(state, monkey_token_type_assign);
  if (state.status) {
    return state;
  }

  arcana_parser_ast_next_token(&state);
  if (state.status) {
    return state;
  }

  state = arcana_parser_expect_token(state, monkey_token_type_number);
  if (state.status) {
    return state;
  }

  arcana_token lit_token = arcana_tokens_data(state.tokens)[state.token_cursor];
  *(monkey_slice *)((char *)data + lit_addr) = {
      .base = lit_token.off,
      .len = lit_token.len,
  };

  arcana_parser_ast_next_token(&state);
  if (state.status) {
    return state;
  }

  state = arcana_parser_expect_token(state, monkey_token_type_semi);
  if (state.status) {
    return state;
  }

  arcana_parser_ast_next_token(&state);
  if (state.status) {
    return state;
  }

  state.last_root_child = let_node;

  return state;
}

arcana_parser_state monkey_parse_file(arcana_parser_state state) {
  while (!arcana_parser_state_done(state)) {
    state = monkey_parse_let(state);

    if (state.status) {
      return state;
    }
  }

  return state;
}
