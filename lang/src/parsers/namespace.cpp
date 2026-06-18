
#include "parsers.h"
#include <sigil.h>

namespace arcana {

sigil_state parse_module(sigil_state state) {
  check_token(module);
  next_token();

  auto [id, root] = alloc_node(ns);

  run_subparser(root, parse_ident, child);

  check_token(lbrace);
  next_token();

  sigil_node *cur = sigil_state_node(state, state.subroot);
  while (true) {
    loop_terminal_token(rbrace);
    run_subparser(cur, parse_declaration, next);
    cur = sigil_ast_nodes(state.ast) + cur->next;
  }

  state.subroot = id;
  return state;
}

} // namespace arcana
