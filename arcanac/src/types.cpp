#include "types.h"
#include "symbol.h"
#include <iostream>
#include <iterator>
#include <ranges>

std::string resolve(const sigil::Overlay<arcana::pass::NamePass::Name> &scopes,
                    const SymbolTable &table,
                    arcana::pass::NamePass::Name name) {

  std::string res;
  std::vector<symbol> syms;
  arcana::pass::NamePass::Name n = name;

  syms.push_back(n._symbol);
  while (n._parent) {
    n = *scopes.resolve(n._parent);

    syms.push_back(n._symbol);
  }

  for (const auto &sym : syms | std::views::reverse) {
    res += table.resolve(sym);
    res += "::";
  }

  if (res.size()) {
    res.resize(res.size() - 2);
  }

  return res;
}

void report_types(const arcana::Tokens &, const arcana::Ast &,
                  const sigil::Overlay<arcana::pass::NamePass::Name> &scopes,
                  const arcana::pass::TypeDefPass &pass) {

  std::cout << "bitsets" << std::endl;
  for (const auto &bitset : pass.bitsets) {

    auto name = scopes.resolve(bitset.node);

    std::string id = resolve(scopes, pass.table, *name);

    std::cout << "  " << id << std::endl;

    for (const auto &c : bitset.cases) {
      auto x = pass.table.resolve(c.sym);

      std::cout << "    " << x << " = " << c.bit << std::endl;
    }
  }
}
