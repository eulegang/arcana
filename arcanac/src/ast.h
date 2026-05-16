#pragma once

#include "arcana.h"
#include <sigil.h>

sigil::Ast<arcana::Node> parse_ast(const sigil::Tokens<arcana::Token> &tokens);

void report_ast(const sigil::Tokens<arcana::Token> &tokens,
                const sigil::Ast<arcana::Node> &tree);
