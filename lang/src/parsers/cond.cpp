#include "parsers.h"
#include <sigil.h>

namespace arcana {
sigil_state parse_cond(sigil_state state) {
  check_token(cond_if);
  next_token();

  auto [id, root] = alloc_node(cond);
  auto [if_id, if_node] = alloc_node(cond_if);

  root->child = if_id;

  run_subparser(if_node, parse_expr, child);
  sigil_node *expr = sigil_state_node(state, if_node->child);

  run_subparser(expr, parse_block, next);

  // if (!token_is(cond_else)) {
  //   state.subroot = id;
  //   return state;
  // }
  //

  sigil_node *cur = if_node;
  while (token_is(cond_else)) {
    next_token();

    if (token_is(cond_if)) {
      next_token();
      auto [else_if_id, else_if_node] = alloc_node(cond_else_if);
      cur->next = else_if_id;

      run_subparser(else_if_node, parse_expr, child) sigil_node *expr =
          sigil_state_node(state, if_node->child);
      run_subparser(expr, parse_block, next);
    } else {
      auto [else_id, else_node] = alloc_node(cond_else);
      run_subparser(else_node, parse_block, child);
    }
  }

  state.subroot = id;
  return state;
}
} // namespace arcana
