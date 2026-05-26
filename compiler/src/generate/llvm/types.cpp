#include "arcana/types.h"
#include "../../generate.h"
#include "symbol.h"
#include <algorithm>
#include <format>
#include <ranges>
#include <stdexcept>
#include <utility>

using namespace arcana::types;

namespace gen {
const char *prim_name(uint32_t id) {
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

bool llvm::is_definable(type_id tid) {
  if (tid.category() == type_id::cat::prim)
    return true;

  if (alloc_names.contains(tid))
    return true;

  switch (tid.category()) {
  case type_id::cat::meta:
  case type_id::cat::prim:
  case type_id::cat::bs:
  case type_id::cat::en:
  case type_id::cat::derive:
    return true;
  case type_id::cat::st: {
    auto st = types.base.lookup<Struct>(tid);
    return std::ranges::all_of(
        st.fields, [&](Struct::Field field) { return is_definable(field.ty); });

  } break;

  case type_id::cat::fn: {
    auto fn = types.base.lookup<Fn>(tid);
    return is_definable(fn.err) || is_definable(fn.ret) ||
           std::ranges::all_of(fn.params,
                               [&](type_id id) { return is_definable(id); });
  }

  case type_id::cat::alias:
  case type_id::cat::ref:
    return false;
  }

  return false;
}

std::string llvm::type_name(arcana::types::type_id tid) {

  switch (tid.category()) {
  case type_id::cat::meta:
  case type_id::cat::bs:
  case type_id::cat::en:
  case type_id::cat::st:
  case type_id::cat::alias:
    break;

  case type_id::cat::ref:
    break;

  case type_id::cat::derive: {
    auto derive = types.base.lookup<Derive>(tid);
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
    auto fn = types.base.lookup<Fn>(tid);

    if (fn.err) {
      throw std::logic_error("unimplemented");
    } else {
      std::string buf;
      buf += fn.ret ? type_name(fn.ret) : "void";
      buf += "(";

      auto it = fn.params.begin();

      if (it != fn.params.end()) {
        buf += type_name(*it++);
      }

      while (it != fn.params.end()) {
        buf += ", ";
        buf += type_name(*it++);
      }

      buf += ")";

      return buf;
    }
  }
  }

  return std::format("!!! {}", (int)tid.category());
}

std::string llvm::name_of(uint16_t node) {
  std::string res;
  std::vector<symbol> syms;

  auto name = names.overlay.resolve(node);

  syms.push_back(name->_symbol);
  while (name->_parent != 0xFFFF) {
    name = names.overlay.resolve(name->_parent);
    if (!name)
      break;
    syms.push_back(name->_symbol);
  }

  std::reverse(syms.begin(), syms.end());

  for (const auto &sym : syms) {
    res += names.symbol_table.resolve(sym);
    res += ".";
  }

  if (res.size()) {
    res.resize(res.size() - 1);
  }

  return res;
}
} // namespace gen
