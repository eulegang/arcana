#include "../generate.h"
#include "arcana/types.h"
#include "symbol.h"
#include <algorithm>
#include <format>
#include <ranges>
#include <stdexcept>
#include <utility>

using namespace arcana::types;

namespace gen {
void llvm::generate(std::ostream &out) {

  for (const auto &[node_id, tid] : types.ids) {
    switch (tid.category()) {
    case type_id::cat::meta:
    case type_id::cat::bs:
      gen(out, node_id, tid, types.base.lookup<BitSet>(tid));
      break;

    case type_id::cat::en:
      gen(out, node_id, tid, types.base.lookup<Enumeration>(tid));
      break;

    case type_id::cat::st:
      gen(out, node_id, tid, types.base.lookup<Struct>(tid));
      break;

    case type_id::cat::alias:
      gen(out, node_id, tid, types.base.lookup<Alias>(tid));
      break;

    case type_id::cat::prim:
    case type_id::cat::ref:
    case type_id::cat::derive:
    case type_id::cat::fn:
      break;
    }
  }

  out << "; pending" << std::endl;
  for (const auto &[node_id, tid] : pending) {
    std::string tname = std::format("$arcana.{}", name_of(node_id));
    out << "; " << tname << ", " << tid.id() << std::endl;
  }
}

void llvm::gen(std::ostream &out, uint16_t node_id, arcana::types::type_id tid,
               arcana::types::BitSet &bs) {
  std::string tname = std::format("$arcana.{}", name_of(node_id));

  alloc_names.insert({tid, tname});

  out << "%" << tname << " = type { i" << bs.size << " }" << std::endl;

  for (auto const &c : bs.cases) {
    uint64_t val = 1 << c.bit;
    const char *var = names.symbol_table.resolve(c.sym);

    out << "@" << tname << "_" << var << " = internal constant i" << bs.size
        << " " << val << std::endl;
  }
}

void llvm::gen(std::ostream &out, uint16_t node_id, arcana::types::type_id tid,
               arcana::types::Enumeration &en) {
  std::string tname = std::format("$arcana.{}", name_of(node_id));

  alloc_names.insert({tid, tname});

  out << "%" << tname << " = type { i" << en.size << " }" << std::endl;

  for (const auto &c : en.cases) {
    const char *var = names.symbol_table.resolve(c.sym);

    out << "@" << tname << "_" << var << " = internal constant "
        << "%" << tname << " { i" << en.size << " " << c.pattern << " }"
        << std::endl;
  }
}

void llvm::gen(std::ostream &out, uint16_t node_id, arcana::types::type_id tid,
               arcana::types::Struct &st) {

  if (!is_definable(tid)) {
    pending.push_back(std::make_pair(node_id, tid));
    return;
  }

  std::string tname = std::format("$arcana.{}", name_of(node_id));

  out << "%" << tname << " = type { ";

  auto it = st.fields.begin();

  if (it != st.fields.end()) {
    out << type_name((*it++).ty);
  }

  while (it != st.fields.end()) {
    out << ", " << type_name((*it++).ty);
  }

  out << " }" << std::endl;
}

void llvm::gen(std::ostream &out, uint16_t node_id, arcana::types::type_id tid,
               arcana::types::Alias &alias) {
  if (!is_definable(alias.id)) {
    pending.push_back(std::make_pair(node_id, tid));
    return;
  }

  std::string tname = std::format("$arcana.{}", name_of(node_id));

  switch (alias.id.category()) {
  case type_id::cat::meta:
  case type_id::cat::bs:
  case type_id::cat::en:
  case type_id::cat::st:
  case type_id::cat::ref:
  case type_id::cat::derive:
    return;
  case type_id::cat::fn:
  case type_id::cat::alias:
    out << "%" << tname << " = type " << type_name(alias.id) << std::endl;
    break;
  case type_id::cat::prim:
    alloc_names.insert({{tid, tname}});
    out << "%" << tname << " = type " << type_name(alias.id) << std::endl;

    break;
  }
}
} // namespace gen
