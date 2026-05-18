#include "../reports.h"
#include "symbol.h"
#include <chroma.h>
#include <iostream>
#include <iterator>
#include <ranges>

void report::types(const arcana::Tokens &, const arcana::Ast &,
                   const arcana::pass::NamePass::Overlay &scopes,
                   const arcana::pass::TypeDefPass &pass) {

  std::cout << chroma::purple << "summary" << std::endl;

  std::cout << chroma::purple << "  bitsets" << std::endl;
  for (const auto &bitset : pass.bitsets) {

    auto name = scopes.resolve(bitset.node);

    std::string id = resolve(scopes, pass.table, *name, "::");

    std::cout << "    " << chroma::green << id << chroma::clear << " ("
              << chroma::yellow << bitset.size << chroma::clear << ")"
              << std::endl;

    for (const auto &c : bitset.cases) {
      auto x = pass.table.resolve(c.sym);

      std::cout << "      " << chroma::cyan << x << chroma::clear << " = "
                << chroma::yellow << c.bit << std::endl;
    }
  }

  std::cout << chroma::purple << "  enums" << std::endl;
  for (const auto &en : pass.enums) {

    auto name = scopes.resolve(en.node);

    std::string id = resolve(scopes, pass.table, *name, "::");

    std::cout << "    " << chroma::green << id << chroma::clear << " ("
              << chroma::yellow << en.size << chroma::clear << ")" << std::endl;

    for (const auto &c : en.cases) {
      auto x = pass.table.resolve(c.sym);

      std::cout << "      " << chroma::cyan << x << chroma::clear << " = "
                << chroma::yellow << c.pattern << std::endl;
    }
  }
}
