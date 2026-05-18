#pragma once

#include "arcana.h"
#include "pass/name.h"
#include "pass/types.h"
#include "symbol.h"
#include <sigil.h>

extern bool verbose;

namespace report {

void tokens(const sigil::Tokens<arcana::Token> &tokens);

void ast(const sigil::Tokens<arcana::Token> &tokens,
         const sigil::Ast<arcana::Node> &tree);

void symbols(const SymbolTable &);

void types(const arcana::Tokens &, const arcana::Ast &,
           const sigil::Overlay<arcana::pass::NamePass::Name> &scopes,
           const arcana::pass::TypeDefPass &);

template <typename T>
void overlay(const arcana::Token &, const arcana::Ast &,
             const sigil::Overlay<T> &);
} // namespace report

std::ostream &operator<<(std::ostream &os, const arcana::Node &);
