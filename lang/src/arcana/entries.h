#pragma once

#include "sigil.h"
#include <vector>

namespace arcana {
namespace entry {

struct Foreign {
  sigil_node_id id;
};

struct Body {
  sigil_node_id id;
};

struct Const {
  sigil_node_id id;
};

struct Entries {
  std::vector<Foreign> foreigns;
  std::vector<Body> bodies;
};
} // namespace entry
} // namespace arcana
