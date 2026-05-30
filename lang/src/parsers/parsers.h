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

#define alloc_data(Type)                                                       \
  [&state]() {                                                                 \
    uint16_t id = sigil_state_malloc(&state, sizeof(Type));                    \
    *(Type *)sigil_state_data(state, data) = state.token_cursor;               \
  }

#define run_subparser(node, subparser)                                         \
  state = arcana::subparser(state);                                            \
  if (state.status)                                                            \
    return state;                                                              \
  node->child = state.subroot;

#define swap_branch(node) std::swap(node->child, node->next)

namespace arcana {
sigil_state parse_bitset(sigil_state state);
sigil_state parse_ident(sigil_state state);
sigil_state parse_lit(sigil_state state);
sigil_state parse_namespace(sigil_state state);
sigil_state parse_module(sigil_state state);
sigil_state parse_declaration(sigil_state state);
sigil_state parse_enum(sigil_state state);
sigil_state parse_struct(sigil_state state);
sigil_state parse_alias(sigil_state state);

sigil_state parse_func_sig(sigil_state state);

sigil_state parse_type(sigil_state state);

extern sigil_parser *type_parser;

void init_type_parser(void) __attribute__((constructor));
void deinit_type_parser(void) __attribute__((destructor));
} // namespace arcana
