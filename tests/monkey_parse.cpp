
#include "arcana.h"
#include "monkey.h"
#include "gmock/gmock.h"
#include <cstdint>

#include <cstdio>

#ifndef NDEBUG
#define debug(msg, ...)                                                        \
  do {                                                                         \
    printf(msg "\n" __VA_OPT__(, ) __VA_ARGS__);                               \
  } while (0)

#else
#define debug(msg, ...)
#endif

arcana_parser_state monkey_parse_expr(arcana_parser_state);

arcana_parser_state monkey_parse_expr_infix(arcana_parser_state state) {
  debug("parse_expr_infix");
  arcana_token token = arcana_parser_token(state);
  arcana_parse_node *nodes = arcana_parser_ast_nodes(state.ast);
  void *data = arcana_parser_ast_data(state.ast);

  switch ((monkey_token_type)token.type) {
  case monkey_token_type_ident: {
    uint16_t ident_node = arcana_parser_alloc_node(&state);
    uint16_t ident_addr =
        arcana_parser_ast_malloc(&state, sizeof(monkey_slice));

    nodes[ident_node] = {
        .child = 0,
        .next = 0,
        .offset = ident_addr,
        .type = (uint16_t)monkey_node_type::ident,
    };

    *(monkey_slice *)((char *)data + ident_addr) = {
        .base = token.off,
        .len = token.len,
    };

    arcana_parser_ast_next_token(&state);

  } break;

  case monkey_token_type_number: {
    uint16_t lit_node = arcana_parser_alloc_node(&state);

    uint16_t lit_addr = arcana_parser_ast_malloc(&state, sizeof(monkey_slice));

    nodes[lit_node] = {
        .child = 0,
        .next = 0,
        .offset = lit_addr,
        .type = (uint16_t)monkey_node_type::lit,
    };

    *(monkey_slice *)((char *)data + lit_addr) = {
        .base = token.off,
        .len = token.len,
    };

    arcana_parser_ast_next_token(&state);
  } break;

  case monkey_token_type_lparen: {
    arcana_parser_ast_next_token(&state);
    state = monkey_parse_expr(state);
    if (state.status) {
      return state;
    }

    state = arcana_parser_expect_token(state, monkey_token_type_rparen);

    return state;
  }

  case monkey_token_type_rparen:
  case monkey_token_type_let:
  case monkey_token_type_assign:
  case monkey_token_type_semi:
  case monkey_token_type_mult:
  case monkey_token_type_div:
  case monkey_token_type_plus:
  case monkey_token_type_minus:
    state.status |= 1;
    break;
  }

  return state;
}

arcana_parser_state monkey_parse_expr_l_term(arcana_parser_state state,
                                             uint16_t expr_id) {
  debug("parse_expr_l_term");
  state.subroot = expr_id;
  return state;
}

arcana_parser_state monkey_parse_expr_l2(arcana_parser_state state,
                                         uint16_t expr_id) {
  debug("parse_expr_l2");
  return monkey_parse_expr_l_term(state, expr_id);
}

arcana_parser_state monkey_parse_expr_l1(arcana_parser_state state,
                                         uint16_t expr_id) {
  debug("parse_expr_l1");
  auto token = arcana_parser_token(state);

  if (token.type == monkey_token_type_plus ||
      token.type == monkey_token_type_minus) {
    arcana_parser_ast_next_token(&state);
    if (state.status)
      return state;

    uint16_t root = arcana_parser_alloc_node(&state);
    arcana_parse_node *node = arcana_parser_ast_nodes(state.ast) + root;
    node->offset = 0xFFFF;
    node->child = 0;
    node->next = 0;
    node->type = token.type == monkey_token_type_plus
                     ? (uint16_t)monkey_node_type::add
                     : (uint16_t)monkey_node_type::sub;

  } else {
    return monkey_parse_expr_l2(state, expr_id);
  }

  return state;
}

arcana_parser_state monkey_parse_expr(arcana_parser_state state) {
  debug("parse_expr");
  state = monkey_parse_expr_infix(state);
  uint16_t expr_id = state.subroot;

  arcana_parser_state attempt = monkey_parse_expr_l1(state, expr_id);

  if (attempt.status) {
    return state;
  }

  return attempt;
}

arcana_parser_state monkey_parse_let(arcana_parser_state state) {
  debug("parse_let");
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

  state.subroot = let_node;

  return state;
}

arcana_parser_state monkey_parse_file(arcana_parser_state state) {
  debug("parse_file");
  uint16_t last = 0xFFFF;

  while (!arcana_parser_state_done(state)) {
    state = monkey_parse_let(state);

    if (last != 0xFFFF) {
      arcana_parse_node *node = arcana_parser_ast_nodes(state.ast) + last;
      node->next = state.subroot;
    }

    last = state.subroot;

    if (state.status) {
      return state;
    }
  }

  return state;
}
