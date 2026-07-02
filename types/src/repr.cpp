#include "arcana/typebase.h"
#include <sstream>

using namespace arcana::types;

std::string Typebase::repr(type_id id) const {
  std::stringstream ss;

  switch (id.category()) {
  case type_id::cat::meta:
    switch (id.id()) {
    case 0:
      return "null";
    case 1:
      return "poison";
    default:
      break;
    }
    break;
  case type_id::cat::bs: {
    auto bs = bitsets[id.id()];

    ss << "bitset(" << bs.size << ") { ";

    auto it = bs.cases.begin();

    if (it != bs.cases.end()) {
      auto [sym, bit] = *it++;
      ss << table.resolve(sym) << " = " << bit;
    }

    while (it != bs.cases.end()) {
      ss << ", ";
      auto [sym, bit] = *it++;
      ss << table.resolve(sym) << " = " << bit;
    }

    ss << "}";

    return ss.str();
  } break;

  case type_id::cat::en: {
    auto en = enums[id.id()];

    ss << "bitset(" << en.size << ") { ";

    auto it = en.cases.begin();

    if (it != en.cases.end()) {
      auto [sym, pattern] = *it++;
      ss << table.resolve(sym) << " = " << pattern;
    }

    while (it != en.cases.end()) {
      ss << ", ";
      auto [sym, bit] = *it++;
      ss << table.resolve(sym) << " = " << bit;
    }

    ss << "}";

    return ss.str();
  } break;

  case type_id::cat::st: {
    auto en = structs[id.id()];

    ss << "record { ";

    auto it = en.fields.begin();

    if (it != en.fields.end()) {
      auto [sym, tid] = *it++;
      ss << table.resolve(sym) << ": " << repr(tid);
    }

    while (it != en.fields.end()) {
      ss << ", ";
      auto [sym, tid] = *it++;
      ss << table.resolve(sym) << ": " << repr(tid);
    }

    ss << "}";

    return ss.str();

  } break;

  case type_id::cat::prim: {
    return table.resolve(primitives[id.id()].sym);
  } break;

  case type_id::cat::derive: {
    auto [ty, tid] = derives[id.id()];
    switch (ty) {
    case Derive::Type::Pointer:
      ss << "*";
      break;
    case Derive::Type::Slice:
      ss << "[]";
      break;
    }

    ss << repr(tid);

    return ss.str();
  }
  case type_id::cat::alias: {
    return repr(aliases[id.id()].id);
  } break;

  case type_id::cat::fn: {
    auto fn = fns[id.id()];
    auto it = fn.params.begin();

    ss << "(";

    if (it != fn.params.end()) {
      ss << repr(*it++);
    }

    while (it != fn.params.end()) {
      ss << ", " << repr(*it++);
    }

    ss << ")";

    if (fn.err) {
      ss << " -> " << repr(fn.err) << "!" << repr(fn.ret);
    } else if (fn.ret) {
      ss << " -> " << repr(fn.ret);
    }
    return ss.str();
  } break;
  }

  return "!!!";
}
