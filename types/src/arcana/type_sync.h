#pragma once

#include "arcana/space.h"
#include "arcana/type_id.h"
#include "arcana/typebase.h"

#include <sigil.h>
#include <vector>

namespace arcana::types {
struct TypeSync {
  using Link = std::pair<sigil_node_id, sigil_node_id>;
  using Assoc = std::pair<sigil_node_id, type_id>;
  using Member = std::tuple<sigil_node_id, sigil_node_id, symbol>;

  void push(sigil_node_id id);
  void link(sigil_node_id dst, sigil_node_id src);
  void set(sigil_node_id dst, type_id src);
  void hint(sigil_node_id dst, type_id src);
  void member(sigil_node_id dst, sigil_node_id src, symbol ref);

  void compress();

  const std::span<Assoc> facts() { return _facts; }

  TypeSync(const Typebase &base) : base{base} {}

  std::vector<sigil_node_id> linked(sigil_node_id) const;

private:
  util::Space<Link> _links;
  util::Space<Assoc> _facts;
  util::Space<Assoc> _hints;
  util::Space<sigil_node_id> _unknowns;
  util::Space<Member> _members;
  const Typebase &base;
};
} // namespace arcana::types
