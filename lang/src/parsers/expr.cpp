#include "parsers.h"
#include <sigil.h>

namespace arcana {
sigil_parser *expr_parser;

sigil_state parse_expr(sigil_state state) {
  return sigil_parser_parse_expr(expr_parser, state, (size_t)expr_perc::LOWEST);
}

sigil_state parse_expr_init(sigil_state state) {
  auto [id, root] = alloc_node(expr);

  state =
      sigil_parser_parse_expr(expr_parser, state, (size_t)expr_perc::LOWEST);
  root->child = state.subroot;

  state.subroot = id;
  return state;
}

void init_expr_parser(void) {
  extern bool terminal(sigil_token_type type);
  expr_parser =
      sigil_parser_init((uint16_t)Token::END + 1, terminal, parse_expr_init);

  sigil_parser_slots(expr_parser)[(uint16_t)Token::ident] = {
      .prefix = parse_ident,
      .postfix = 0,
      .infix = 0,
      .perc = 0,
  };

  sigil_parser_slots(expr_parser)[(uint16_t)Token::integer] = {
      .prefix = parse_lit,
      .postfix = 0,
      .infix = 0,
      .perc = 0,
  };

  sigil_parser_slots(expr_parser)[(uint16_t)Token::dot] = {
      .prefix = 0,
      .postfix = 0,
      .infix = parse_member,
      .perc = (size_t)expr_perc::dot,
  };

  sigil_parser_slots(expr_parser)[(uint16_t)Token::lparen] = {
      .prefix = parse_group,
      .postfix = 0,
      .infix = parse_func_call,
      .perc = (size_t)expr_perc::call,
  };
}

void deinit_expr_parser(void) { sigil_parser_deinit(expr_parser); }

} // namespace arcana
