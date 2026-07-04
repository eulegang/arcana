#pragma once

#include "arcana/type_id.h"

#include <sigil.h>
#include <vector>

namespace arcana::types {
struct TypeSync {
  using Link = std::pair<sigil_node_id, sigil_node_id>;
  using Assoc = std::pair<sigil_node_id, type_id>;

  void push(sigil_node_id id);
  void link(sigil_node_id dst, sigil_node_id src);
  void set(sigil_node_id dst, type_id src);
  void hint(sigil_node_id dst, type_id src);

  void compress();

  const std::vector<Assoc> &facts() { return _facts; }

private:
  std::vector<Link> _links;
  std::vector<Assoc> _facts;
  std::vector<Assoc> _hints;
  std::vector<sigil_node_id> _unknowns;

  std::vector<sigil_node_id> linked(sigil_node_id);
};
} // namespace arcana::types
