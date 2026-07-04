#pragma once

#include "arcana.h"
#include "arcana/pass.h"
#include "arcana/types.h"
#include "symbol.h"
#include <sigil.h>

extern bool verbose;

namespace report {

void tokens(const sigil::Tokens<arcana::Token> &tokens);

void ast(const sigil::Tokens<arcana::Token> &tokens,
         const sigil::Ast<arcana::Node> &tree);

void symbols(const SymbolTable &);

void names(const arcana::Ast &, const SymbolTable &,
           const sigil::Overlay<arcana::pass::NamePass::Name> &);

void types(const arcana::Tokens &, const arcana::Ast &,
           const arcana::pass::NamePass::Overlay &,
           const arcana::types::Typebase &, const arcana::types::TypeDefPass &);

void diagnostics(std::string_view, const arcana::Tokens &tokens,
                 arcana::Diagnostics &diagnostics);

std::string resolve(const arcana::pass::NamePass::Overlay &scopes,
                    const SymbolTable &table, arcana::pass::NamePass::Name name,
                    std::string_view join);

} // namespace report

std::ostream &operator<<(std::ostream &os, const arcana::Node &);
