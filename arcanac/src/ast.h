#pragma once

#include "arcana.h"
#include <sigil.h>

sigil::Ast<arcana::Node> parse_ast(const sigil::Tokens<arcana::Token> &tokens);
sigil::Tokens<arcana::Token> tokenize(std::string_view content);
