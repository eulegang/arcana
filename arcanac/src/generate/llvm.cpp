#include "../generate.h"
#include "arcana/types.h"
#include "symbol.h"

namespace gen {
void llvm::generate(std::ostream &out) {
  generate_bitsets(out);
  generate_enumerations(out);
}

void llvm::generate_bitsets(std::ostream &out) {
  using namespace arcana::types;

  for (const auto &[node_id, tid] : types.ids) {
    if (tid.category() == arcana::types::type_id::cat::bs) {
      BitSet &bs = types.base.lookup<BitSet>(tid);
      std::string name = name_of(node_id);

      out << "%$arcana." << name << " = type { i" << bs.size << " }"
          << std::endl;

      for (auto const &c : bs.cases) {
        uint64_t val = 1 << c.bit;
        const char *var = names.symbol_table.resolve(c.sym);

        out << "@$arcana." << name << "_" << var << " = internal constant i"
            << bs.size << " " << val << std::endl;
      }
    }
  }
}

void llvm::generate_enumerations(std::ostream &out) {
  using namespace arcana::types;

  for (const auto &[node_id, tid] : types.ids) {
    if (tid.category() == arcana::types::type_id::cat::en) {

      Enumeration &en = types.base.lookup<Enumeration>(tid);
      std::string name = name_of(node_id);

      out << "%$arcana." << name << " = type { i" << en.size << " }"
          << std::endl;

      for (const auto &c : en.cases) {
        const char *var = names.symbol_table.resolve(c.sym);

        out << "@$arcana." << name << "_" << var << " = internal constant "
            << "%$arcana." << name << " { i" << en.size << " " << c.pattern
            << " }" << std::endl;
      }
    }
  }
}

std::string llvm::name_of(uint16_t node) {
  std::string res;
  std::vector<symbol> syms;

  auto name = names.overlay.resolve(node);

  syms.push_back(name->_symbol);
  while (name->_parent != 0xFFFF) {
    name = names.overlay.resolve(name->_parent);
    if (!name)
      break;
    syms.push_back(name->_symbol);
  }

  std::reverse(syms.begin(), syms.end());

  for (const auto &sym : syms) {
    res += names.symbol_table.resolve(sym);
    res += "_";
  }

  if (res.size()) {
    res.resize(res.size() - 1);
  }

  return res;
}

} // namespace gen
