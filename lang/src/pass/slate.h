#pragma once

#include "../arcana.h"
#include "../arcana/types.h"
#include "sigil.h"

#include <variant>

namespace arcana {
namespace types {
struct TypeSlate {
  using Value = std::variant<uint16_t, type_id, std::monostate>;
  struct Slot {
    sigil_node_id node_id;
    Value value;
  };

  std::vector<Slot> slots;

  void clear();
  void push(sigil_node_id id);
  void link(sigil_node_id dst, sigil_node_id src);
  void set(sigil_node_id dst, type_id src);
};
} // namespace types
} // namespace arcana
