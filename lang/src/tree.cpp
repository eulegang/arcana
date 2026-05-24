#include "arcana.h"
#include "parsers/parsers.h"
#include <cstdint>
#include <sigil.h>

namespace arcana {
sigil_parser *parser;

bool terminal(sigil_token_type type) {
  return type == (sigil_token_type)Token::semi;
}

sigil_state init(sigil_state state) {

  uint16_t last = 0xFFFF;

  while (!sigil_state_done(state)) {
    state = parse_declaration(state);

    if (last != 0xFFFF) {
      sigil_node *node = sigil_ast_nodes(state.ast) + last;
      node->next = state.subroot;
    } else {
      sigil_node *node = sigil_ast_nodes(state.ast);
      node[0] = node[state.subroot];
      last = 0;
    }

    last = state.subroot;

    if (state.status) {
      return state;
    }
  }

  state.subroot = last;

  return state;
}

__attribute__((constructor)) void init_parser(void) {
  parser = sigil_parser_init((uint16_t)Token::END + 1, terminal, init);

  sigil_parser_slots(parser)[(uint16_t)Token::integer] = {
      .prefix = parse_lit,
      .postfix = 0,
      .infix = 0,
      .perc = 0,
  };
}

__attribute__((destructor)) void deinit_parser(void) {
  sigil_parser_deinit(parser);
}

} // namespace arcana
