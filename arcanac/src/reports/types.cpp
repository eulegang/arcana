#include "../reports.h"
#include "symbol.h"
#include <iostream>
#include <iterator>
#include <ranges>

void report::types(const arcana::Tokens &, const arcana::Ast &,
                   const arcana::pass::NamePass::Overlay &scopes,
                   const arcana::pass::TypeDefPass &pass) {

  std::cout << "bitsets" << std::endl;
  for (const auto &bitset : pass.bitsets) {

    auto name = scopes.resolve(bitset.node);

    std::string id = resolve(scopes, pass.table, *name, "::");

    std::cout << "  " << id << " (" << bitset.size << ")" << std::endl;

    for (const auto &c : bitset.cases) {
      auto x = pass.table.resolve(c.sym);

      std::cout << "    " << x << " = " << c.bit << std::endl;
    }
  }

  std::cout << "enums" << std::endl;
  for (const auto &en : pass.enums) {

    auto name = scopes.resolve(en.node);

    std::string id = resolve(scopes, pass.table, *name, "::");

    std::cout << "  " << id << " (" << en.size << ")" << std::endl;

    for (const auto &c : en.cases) {
      auto x = pass.table.resolve(c.sym);

      std::cout << "    " << x << " = " << c.pattern << std::endl;
    }
  }
}
