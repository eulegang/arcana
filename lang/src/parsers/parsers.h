#pragma once

#include "../arcana.h"
#include <sigil.h>

typedef uint16_t node_id;
typedef uint16_t data_id;

#define token_code(T) (uint32_t)arcana::Token::T
#define node_code(T) (uint16_t)arcana::Node::T

#define check_token(T)                                                         \
  {                                                                            \
    sigil_token token = sigil_state_token(state);                              \
    if (token.type != token_code(T)) {                                         \
      state.status |= 4;                                                       \
      return state;                                                            \
    }                                                                          \
  }

#define next_token()                                                           \
  sigil_state_next(&state);                                                    \
  if (state.status)                                                            \
    return state;

#define loop_terminal_token(T)                                                 \
  {                                                                            \
    sigil_token token = sigil_state_token(state);                              \
    if (token.type == token_code(T)) {                                         \
      sigil_state_next(&state);                                                \
      break;                                                                   \
    }                                                                          \
  }

#define alloc_node(Type)                                                       \
  [&state]() {                                                                 \
    uint16_t id = sigil_state_alloc_node(&state);                              \
    sigil_node *node = sigil_state_node(state, id);                            \
    *node = {                                                                  \
        .child = 0,                                                            \
        .next = 0,                                                             \
        .offset = 0xFFFF,                                                      \
        .type = node_code(Type),                                               \
    };                                                                         \
    struct {                                                                   \
      uint16_t id;                                                             \
      sigil_node *node;                                                        \
    } result = {.id = id, .node = node};                                       \
    return result;                                                             \
  }();

#define run_subparser(node, subparser, slot)                                   \
  state = arcana::subparser(state);                                            \
  if (state.status)                                                            \
    return state;                                                              \
  node->slot = state.subroot;

#define token_is(Type) ((Token)sigil_state_token(state).type == Token::Type)

namespace arcana {
enum class expr_perc : size_t {
  LOWEST,

  dot,
  call,

  cmp,

  addsub,

  HIGHEST,
};

sigil_state parse_bitset(sigil_state state);
sigil_state parse_ident(sigil_state state);
sigil_state parse_lit(sigil_state state);
sigil_state parse_namespace(sigil_state state);
sigil_state parse_module(sigil_state state);
sigil_state parse_declaration(sigil_state state);
sigil_state parse_enum(sigil_state state);
sigil_state parse_struct(sigil_state state);
sigil_state parse_alias(sigil_state state);

sigil_state parse_func(sigil_state state);
sigil_state parse_foreign(sigil_state state);
sigil_state parse_func_type(sigil_state state);

sigil_state parse_block(sigil_state state);
sigil_state parse_statement(sigil_state state);
sigil_state parse_cond(sigil_state state);

sigil_state parse_expr(sigil_state state);
sigil_state parse_group(sigil_state state);
sigil_state parse_func_call(sigil_state state, sigil_node_id);
sigil_state parse_member(sigil_state state, sigil_node_id);
sigil_state parse_binop(sigil_state state, sigil_node_id);
sigil_state parse_preop(sigil_state state);

sigil_state parse_type(sigil_state state);

extern sigil_parser *type_parser;
extern sigil_parser *expr_parser;

void init_type_parser(void) __attribute__((constructor));
void deinit_type_parser(void) __attribute__((destructor));
void init_expr_parser(void) __attribute__((constructor));
void deinit_expr_parser(void) __attribute__((destructor));
} // namespace arcana
