#include "../generate.h"

using Ast = arcana::Ast;

void gen::EntriesComponent::generate() {
  for (const auto &f : unit.entries.constvar) {
    Ast::Node binding = unit.ast[f.id];
    Ast::Node ident = unit.ast[binding.child];
    Ast::Node ty = unit.ast[ident.next];
    unit.ast[ty.next];
    // Ast::Node val = unit.ast[ty.next];

    std::string name = name_of(unit, binding.child);

    emitter.global(std::format("arcana.{}", name), {"i1", 1});
  }
}
