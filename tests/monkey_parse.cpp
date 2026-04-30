
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

#define token(T) (arcana_token_type) monkey_token_type::T

arcana_parser_state monkey_parse_expr(arcana_parser_state);

arcana_parser_state monkey_parse_expr_infix(arcana_parser_state state) {
  debug("parse_expr_infix");
  arcana_token token = arcana_parser_token(state);
  arcana_parse_node *nodes = arcana_parser_ast_nodes(state.ast);
  void *data = arcana_parser_ast_data(state.ast);

  switch ((monkey_token_type)token.type) {
  case monkey_token_type::ident: {
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

    state.subroot = ident_node;

  } break;

  case monkey_token_type::number: {
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

    state.subroot = lit_node;
  } break;

  case monkey_token_type::lparen: {
    arcana_parser_ast_next_token(&state);
    state = monkey_parse_expr(state);
    if (state.status) {
      return state;
    }

    state =
        arcana_parser_expect_token(state, (uint16_t)monkey_token_type::rparen);

    return state;
  }

  case monkey_token_type::rparen:
  case monkey_token_type::let:
  case monkey_token_type::assign:
  case monkey_token_type::semi:
  case monkey_token_type::mult:
  case monkey_token_type::div:
  case monkey_token_type::plus:
  case monkey_token_type::minus:
    debug("hit error %d", token.type);
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
  auto token = arcana_parser_token(state);

  if ((monkey_token_type)token.type == monkey_token_type::mult ||
      (monkey_token_type)token.type == monkey_token_type::div) {
    arcana_parser_ast_next_token(&state);
    if (state.status)
      return state;

    uint16_t root = arcana_parser_alloc_node(&state);
    arcana_parse_node *node = arcana_parser_ast_nodes(state.ast) + root;
    node->offset = 0xFFFF;
    node->child = 0;
    node->next = 0;
    node->type = (monkey_token_type)token.type == monkey_token_type::mult
                     ? (uint16_t)monkey_node_type::mult
                     : (uint16_t)monkey_node_type::div;

    state.subroot = root;
  } else {
    return monkey_parse_expr_l_term(state, expr_id);
  }

  return monkey_parse_expr_l_term(state, expr_id);
}

arcana_parser_state monkey_parse_expr_l1(arcana_parser_state state,
                                         uint16_t expr_id) {
  debug("parse_expr_l1");
  auto token = arcana_parser_token(state);

  if ((monkey_token_type)token.type == monkey_token_type::plus ||
      (monkey_token_type)token.type == monkey_token_type::minus) {
    arcana_parser_ast_next_token(&state);
    if (state.status)
      return state;

    uint16_t root = arcana_parser_alloc_node(&state);
    arcana_parse_node *node = arcana_parser_ast_nodes(state.ast) + root;
    node->offset = 0xFFFF;
    node->child = 0;
    node->next = 0;
    node->type = (monkey_token_type)token.type == monkey_token_type::plus
                     ? (uint16_t)monkey_node_type::add
                     : (uint16_t)monkey_node_type::sub;

    state.subroot = root;

  } else {
    return monkey_parse_expr_l2(state, expr_id);
  }

  return state;
}

arcana_parser_state monkey_parse_expr(arcana_parser_state state) {
  debug("parse_expr");

  arcana_token token = arcana_parser_token(state);
  debug("token found: %d", token.type);

  state = monkey_parse_expr_infix(state);

  if (state.status)
    return state;

  uint16_t expr_id = state.subroot;

  arcana_parser_state attempt = monkey_parse_expr_l1(state, expr_id);
  if (attempt.status) {
    return attempt;
  }

  return state;
}

arcana_parser_state monkey_parse_let(arcana_parser_state state) {
  debug("parse_let");
  arcana_parse_node *nodes = arcana_parser_ast_nodes(state.ast);
  void *data = arcana_parser_ast_data(state.ast);

  uint16_t let_node = arcana_parser_alloc_node(&state);
  uint16_t ident_node = arcana_parser_alloc_node(&state);

  uint16_t ident_addr = arcana_parser_ast_malloc(&state, sizeof(monkey_slice));

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
      .next = 0,
      .offset = ident_addr,
      .type = (uint16_t)monkey_node_type::ident,
  };

  state = arcana_parser_expect_token(state, (uint16_t)monkey_token_type::let);
  if (state.status) {
    return state;
  }

  arcana_parser_ast_next_token(&state);
  if (state.status) {
    return state;
  }

  state = arcana_parser_expect_token(state, (uint16_t)monkey_token_type::ident);
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

  state =
      arcana_parser_expect_token(state, (uint16_t)monkey_token_type::assign);
  if (state.status) {
    return state;
  }

  arcana_parser_ast_next_token(&state);
  if (state.status) {
    return state;
  }

  arcana_parser_state expr_state = monkey_parse_expr(state);
  if (expr_state.status) {
    return expr_state;
  }

  nodes[ident_node].next = expr_state.subroot;
  debug("[let] found subroot %d", expr_state.subroot);

  state =
      arcana_parser_expect_token(expr_state, (uint16_t)monkey_token_type::semi);
  if (state.status) {
    return state;
  }

  arcana_parser_ast_next_token(&state);
  if (state.status) {
    return state;
  }

  state.subroot = let_node;

  debug("endof parse_let");
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
