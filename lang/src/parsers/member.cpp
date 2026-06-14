#include "parsers.h"

namespace arcana {
sigil_state parse_member(sigil_state state, sigil_node_id prior) {
  check_token(dot);
  next_token();

  state = sigil_parser_parse_expr(expr_parser, state, (size_t)expr_perc::dot);
  if (state.status)
    return state;

  auto [id, root] = alloc_node(member);

  root->child = prior;
  sigil_state_node(state, prior)->next = state.subroot;

  state.subroot = id;
  return state;
}
} // namespace arcana
