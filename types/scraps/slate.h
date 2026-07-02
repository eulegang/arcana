#pragma once

#include "../arcana.h"
#include "../arcana/types.h"
#include "sigil.h"

#include <variant>

namespace arcana {
namespace types {
struct TypeSlate {

  struct Link {
    sigil_node_id a;
    sigil_node_id b;
  };

  struct Fact {
    sigil_node_id node;
    type_id tid;
  };

  void push(sigil_node_id id);
  void link(sigil_node_id dst, sigil_node_id src);
  void set(sigil_node_id dst, type_id src);
  void hint(sigil_node_id dst, type_id src);

  void compress();

  const std::vector<Fact> &facts() { return _facts; }

private:
  std::vector<Link> _links;
  std::vector<Fact> _facts;
  std::vector<Fact> _hints;
  std::vector<sigil_node_id> _unknowns;

  std::vector<sigil_node_id> linked(sigil_node_id);
};
} // namespace types
} // namespace arcana
