#include "arcana/types.h"
#include "../generate.h"

using namespace arcana::types;
using Ast = arcana::Ast;

void gen::TypesComponent::generate() { visit(0); }

void gen::TypesComponent::visit(sigil_node_id cur) {
  Ast::Node node = unit.ast[cur];

  auto data = unit.overlays.types.resolve(cur);

  if (data) {
    auto tid = *data;
    switch (tid.category()) {
    case type_id::cat::meta:
    case type_id::cat::bs:
      gen(cur, tid, unit.types.lookup<BitSet>(tid));
      break;

    case type_id::cat::en:
      gen(cur, tid, unit.types.lookup<Enumeration>(tid));
      break;

    case type_id::cat::st:
      gen(cur, tid, unit.types.lookup<Struct>(tid));
      break;

    case type_id::cat::alias:
      gen(cur, tid, unit.types.lookup<Alias>(tid));
      break;

    case type_id::cat::prim:
    case type_id::cat::derive:
    case type_id::cat::fn:
      break;
    }
  }

  if (node.child) {
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
                              const arcana::types::Struct &bs) {}
void gen::TypesComponent::gen(sigil_node_id node, arcana::types::type_id,
                              const arcana::types::Alias &bs) {}
