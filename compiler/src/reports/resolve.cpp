#include "../reports.h"
#include <ranges>

std::string report::resolve(const arcana::pass::NamePass::Overlay &scopes,
                            const SymbolTable &table,
                            arcana::pass::NamePass::Name name,
                            std::string_view join) {

  std::string res;
  std::vector<symbol> syms;
  arcana::pass::NamePass::Name n = name;

  syms.push_back(n._symbol);
  while (n._parent != 0xFFFF) {
    n = *scopes.resolve(n._parent);

    syms.push_back(n._symbol);
  }

  for (const auto &sym : syms | std::views::reverse) {
    res += table.resolve(sym);
    res += join;
  }

  if (res.size()) {
    res.resize(res.size() - 2);
  }

  return res;
}
