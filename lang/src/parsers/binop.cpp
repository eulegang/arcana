#include "parsers.h"
#include <sigil.h>

namespace arcana {
sigil_state parse_binop(sigil_state state, sigil_node_id priv) {
  auto [id, root] = alloc_node(ident);
  root->child = priv;
  // sigil_state_node(state, priv)->next = id;

  expr_perc perc = expr_perc::LOWEST;
  switch ((Token)sigil_state_token(state).type) {
  case Token::eq:
    root->type = node_code(eq);
    perc = expr_perc::cmp;
    break;

  case Token::ne:
    root->type = node_code(ne);
    perc = expr_perc::cmp;
    break;

  default:
    state.status |= 8;
    return state;
  }

  next_token();

  state = sigil_parser_parse_expr(expr_parser, state, (size_t)perc);

  sigil_state_node(state, root->child)->next = state.subroot;

  state.subroot = id;
  return state;
}

sigil_state parse_preop(sigil_state state) {
  auto [id, root] = alloc_node(ident);

  expr_perc perc = expr_perc::LOWEST;
  switch ((Token)sigil_state_token(state).type) {
  case Token::minus:
    root->type = node_code(neg);
    perc = expr_perc::addsub;
    break;

  default:
    state.status |= 8;
    return state;
  }

  next_token();

  state = sigil_parser_parse_expr(expr_parser, state, (size_t)perc);
  if (state.status) {
    return state;
  }
  root->child = state.subroot;

  state.subroot = id;
  return state;
}
} // namespace arcana
