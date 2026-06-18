#include "arcana/types.h"
#include "../generate.h"

#include <sstream>

using namespace arcana::types;
using Ast = arcana::Ast;

void gen::TypesComponent::generate() { visit(0); }

void gen::TypesComponent::visit(sigil_node_id cur) {
  Ast::Node node = unit.ast[cur];

  bool stop = false;
  auto data = unit.overlays.types.resolve(cur);

  if (data) {
    auto tid = *data;
    switch (tid.category()) {
    case type_id::cat::bs:
      gen(cur, tid, unit.types.lookup<BitSet>(tid));
      stop = true;
      break;

    case type_id::cat::en:
      gen(cur, tid, unit.types.lookup<Enumeration>(tid));
      stop = true;
      break;

    case type_id::cat::st:
      gen(cur, tid, unit.types.lookup<Struct>(tid));
      stop = true;
      break;

    case type_id::cat::alias:
      gen(cur, tid, unit.types.lookup<Alias>(tid));
      stop = true;
      break;

    case type_id::cat::prim:
    case type_id::cat::derive:
    case type_id::cat::meta:
    case type_id::cat::fn:
      break;
    }
  }

  if (!stop && node.child) {
    visit(node.child);
  }

  if (node.next) {
    visit(node.next);
  }
}

void gen::TypesComponent::gen(sigil_node_id node, arcana::types::type_id,
                              const arcana::types::BitSet &bs) {
  Ast::Node bs_node = unit.ast[node];
  std::string tname = std::format("$arcana.{}", name_of(unit, bs_node.child));
  auto type = std::format("i{}", bs.size);

  emitter.type(tname, std::format("{{ {} }}", type));

  for (auto const &c : bs.cases) {
    uint64_t val = 1 << c.bit;
    const char *var = unit.symbols.resolve(c.sym);

    emitter.global(
        std::format("{}_{}", tname, var),
        {std::format("%{}", tname), std::format("{{ {} {} }}", type, val)});
  }
}
void gen::TypesComponent::gen(sigil_node_id node, arcana::types::type_id,
                              const arcana::types::Enumeration &en) {

  Ast::Node en_node = unit.ast[node];
  std::string tname = std::format("$arcana.{}", name_of(unit, en_node.child));
  auto type = std::format("i{}", en.size);

  emitter.type(tname, std::format("{{ {} }}", type));
  // out << "%" << tname << " = type { i" << en.size << " }" << std::endl;

  for (const auto &c : en.cases) {
    const char *var = unit.symbols.resolve(c.sym);

    emitter.global(std::format("{}_{}", tname, var),
                   {std::format("%{}", tname),
                    std::format("{{ {} {} }}", type, c.pattern)});
  }
}

void gen::TypesComponent::gen(sigil_node_id node, arcana::types::type_id,
                              const arcana::types::Struct &st) {
  Ast::Node st_node = unit.ast[node];
  std::string tname = std::format("$arcana.{}", name_of(unit, st_node.child));

  std::stringstream out;
  out << "{ ";

  auto it = st.fields.begin();

  if (it != st.fields.end()) {
    out << type_name(unit, (*it++).ty);
  }

  while (it != st.fields.end()) {
    out << ", " << type_name(unit, (*it++).ty);
  }

  out << " }";

  emitter.type(tname, out.str());
}
void gen::TypesComponent::gen(sigil_node_id node, arcana::types::type_id,
                              const arcana::types::Alias &alias) {
  Ast::Node root = unit.ast[node];

  std::string tname = std::format("$arcana.{}", name_of(unit, root.child));

  switch (alias.id.category()) {
  case type_id::cat::meta:
  case type_id::cat::bs:
  case type_id::cat::en:
  case type_id::cat::st:
  case type_id::cat::derive:
    return;
  case type_id::cat::fn:
  case type_id::cat::alias:
  case type_id::cat::prim:
    emitter.type(tname, type_name(unit, alias.id));
    break;
  }
}
