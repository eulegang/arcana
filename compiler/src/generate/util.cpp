#include "../generate.h"
#include "arcana/types.h"

using namespace arcana::types;

const char *gen::prim_name(uint32_t id) {
  switch (id) {
  case 0:
    return "void";

  case 1:
    return "i1";

  case 2:
  case 3:
    return "i8";
  case 4:
  case 5:
    return "i16";
  case 6:
  case 7:
    return "i32";
  case 8:
  case 9:
    return "i64";

  case 10:
    return "float";
  case 11:
    return "double";

  default:
    return NULL;
  }
}

std::string gen::name_of(Unit &unit, uint16_t node) {
  std::string res;
  std::vector<symbol> syms;

  auto name = unit.overlays.names.resolve(node);

  syms.push_back(name->sym);
  while (name->parent != 0xFFFF) {
    name = unit.overlays.names.resolve(name->parent);
    if (!name)
      break;
    syms.push_back(name->parent);
  }

  std::reverse(syms.begin(), syms.end());

  for (const auto &sym : syms) {
    res += unit.symbols.resolve(sym);
    res += ".";
  }

  if (res.size()) {
    res.resize(res.size() - 1);
  }

  return res;
}

std::string gen::type_name(Unit &unit, arcana::types::type_id tid) {

  switch (tid.category()) {
  case type_id::cat::meta:
  case type_id::cat::bs:
  case type_id::cat::en:
  case type_id::cat::st:
  case type_id::cat::alias:
    break;

  case type_id::cat::derive: {
    const Derive &derive = unit.types.lookup<Derive>(tid);
    // types.base.lookup<Derive>(tid);
    switch (derive.ty) {
    case Derive::Type::Pointer:
      return "ptr";
    case Derive::Type::Slice:
      return "{ptr, i64}";
    default:
      throw std::logic_error("invalid value");
    }
  }
  case type_id::cat::prim:
    return prim_name(tid.id());

  case type_id::cat::fn: {
    auto fn = unit.types.lookup<Fn>(tid);

    if (fn.err) {
      throw std::logic_error("unimplemented");
    } else {
      std::string buf;
      buf += fn.ret ? type_name(unit, fn.ret) : "void";
      buf += "(";

      auto it = fn.params.begin();

      if (it != fn.params.end()) {
        buf += type_name(unit, *it++);
      }

      while (it != fn.params.end()) {
        buf += ", ";
        buf += type_name(unit, *it++);
      }

      buf += ")";

      return buf;
    }
  }
  }

  return std::format("!!! {}", (int)tid.category());
}
