
#include "../../generate.h"
#include "arcana.h"
#include "arcana/types.h"
#include "arcana_nodes.h"
#include <cassert>

namespace gen {
using arcana::Ast, arcana::types::type_id;

void llvm::gen_constvar(sigil_node_id node) {
  Ast::Node binding = ast[node];
  Ast::Node ident = ast[binding.child];
  Ast::Node ty = ast[ident.next];
  Ast::Node val = ast[ty.next];

  std::string name = name_of(binding.child);

  emitter.global(std::format("arcana.{}", name), {"i1", 1});
}
} // namespace gen
