;
#include "parsers.h"
#include <sigil.h>

namespace arcana {
sigil_state parse_struct_field(sigil_state state);

sigil_state parse_struct(sigil_state state) {
  check_token(record);
  next_token();

  auto [id, root] = alloc_node(st);

  run_subparser(root, parse_ident, child);
  check_token(lbrace);
  next_token();

  auto [fields_id, fields] = alloc_node(st_fields);
  sigil_state_node(state, root->child)->next = fields_id;

  sigil_node *fields_cur = fields;
  while (true) {
    loop_terminal_token(rbrace);

    state = parse_struct_field(state);

    if (fields_cur == fields) {
      fields_cur->child = state.subroot;
      fields_cur = sigil_ast_nodes(state.ast) + fields_cur->child;
    } else {
      fields_cur->next = state.subroot;
      fields_cur = sigil_ast_nodes(state.ast) + fields_cur->next;
    }

    check_token(semi);
    next_token();
  }

  state.subroot = id;

  return state;
}

sigil_state parse_struct_field(sigil_state state) {
  check_token(ident);
  auto [id, root] = alloc_node(st_field);
  run_subparser(root, parse_ident, child);

  check_token(colon);
  next_token();

  sigil_node *ident_node = sigil_state_node(state, root->child);
  run_subparser(ident_node, parse_type, next);

  state.subroot = id;
  return state;
}

sigil_state parse_alias(sigil_state state) {
  check_token(alias);
  next_token();

  auto [id, root] = alloc_node(alias);
  run_subparser(root, parse_ident, child);

  check_token(assign);
  next_token();

  sigil_node *ident = sigil_state_node(state, id);
  run_subparser(ident, parse_type, next);

  check_token(semi);
  next_token();

  state.subroot = id;
  return state;
}

} // namespace arcana
