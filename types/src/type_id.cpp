#include "arcana/type_id.h"

#include <format>
#include <ostream>

namespace arcana::types {

std::ostream &operator<<(std::ostream &out, const type_id &id) {
  switch (id.category()) {
  case arcana::types::type_id::cat::meta:
    return out << std::format("(meta){}", id.id());
  case arcana::types::type_id::cat::bs:
    return out << std::format("(bs){}", id.id());
  case arcana::types::type_id::cat::en:
    return out << std::format("(en){}", id.id());
  case arcana::types::type_id::cat::st:
    return out << std::format("(st){}", id.id());
  case arcana::types::type_id::cat::prim:
    return out << std::format("(prim){}", id.id());
  case arcana::types::type_id::cat::derive:
    return out << std::format("(derive){}", id.id());
  case arcana::types::type_id::cat::fn:
    return out << std::format("(fn){}", id.id());
  case arcana::types::type_id::cat::alias:
    return out << std::format("(alias){}", id.id());

  default:
    return out << "!!!";
  }
}
} // namespace arcana::types
