#include "arcana.h"

namespace arcana {
Unit::Unit(std::string_view content) : tokens{nullptr}, ast{nullptr} {
  tokens = sigil::Tokens<Token>(content, tokenizer);
  ast = sigil::Ast<Node>(arcana::parser, tokens);
}
} // namespace arcana
