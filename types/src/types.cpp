
#include "arcana/types.h"
#include <sstream>
#include <stdexcept>

namespace arcana {
namespace types {

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

Typebase::Typebase(SymbolTable &table)
    : bitsets{}, enums{}, structs{}, primitives{}, table{table} {

  const Primitive::Flags sign = 0x01;
  const Primitive::Flags f = 0x02;

#define prim(Ty, Size, Stride, Flags)                                          \
  primitives.push_back({                                                       \
      .sym = table.intern(#Ty),                                                \
      .size = Size,                                                            \
      .stride = Stride,                                                        \
      .flags = Flags,                                                          \
  })

  prim(void, 0, 1, 0);
  prim(bool, 1, 1, 0);
  prim(u8, 1, 1, 0);
  prim(i8, 1, 1, sign);
  prim(u16, 2, 2, 0);
  prim(i16, 2, 2, sign);
  prim(u32, 4, 4, 0);
  prim(i32, 4, 4, sign);
  prim(u64, 8, 8, 0);
  prim(i64, 8, 8, sign);
  prim(f32, 4, 4, sign | f);
  prim(f64, 8, 8, sign | f);

#undef prim

  derives.push_back({
      Derive::Type::Slice,
      type_id(type_id::cat::prim, 2),
  });

  derives.push_back({
      Derive::Type::Pointer,
      type_id(type_id::cat::prim, 2),
  });
}

#define intern(Type, Category, Collection)                                     \
  template <> type_id Typebase::intern<Type>(Type arg) {                       \
    type_id id = type_id(type_id::cat::Category, 0);                           \
    for (const auto &record : Collection) {                                    \
      if (arg == record) {                                                     \
        return id;                                                             \
      }                                                                        \
                                                                               \
      id++;                                                                    \
    }                                                                          \
                                                                               \
    Collection.push_back(arg);                                                 \
                                                                               \
    return id;                                                                 \
  }

intern(Fn, fn, fns);
intern(Derive, derive, derives);

#undef intern

#define generate(Type, Category, Collection)                                   \
  template <> Gen<Type> Typebase::generate<Type>() {                           \
    Type value;                                                                \
                                                                               \
    type_id id = type_id(type_id::cat::Category, Collection.size());           \
    Collection.push_back(value);                                               \
                                                                               \
    return {                                                                   \
        .id = id,                                                              \
        .value = Collection.back(),                                            \
    };                                                                         \
  }

generate(Struct, st, structs);
generate(BitSet, bs, bitsets);
generate(Enumeration, en, enums);
generate(Alias, alias, aliases);

#undef generate

#define lookup_impl(Type, Category, Collection)                                \
  template <> Type &Typebase::lookup<Type>(type_id id) {                       \
    if (id.category() != arcana::types::type_id::cat::Category)                \
      throw std::logic_error("invalid type id category expected " #Category);  \
                                                                               \
    return Collection[id.id()];                                                \
  }                                                                            \
  template <> const Type &Typebase::lookup<Type>(type_id id) const {           \
    if (id.category() != arcana::types::type_id::cat::Category)                \
      throw std::logic_error("invalid type id category expected " #Category);  \
                                                                               \
    return Collection[id.id()];                                                \
  }

lookup_impl(BitSet, bs, bitsets);
lookup_impl(Enumeration, en, enums);
lookup_impl(Struct, st, structs);
lookup_impl(Alias, alias, aliases);
lookup_impl(Fn, fn, fns);
lookup_impl(Derive, derive, derives);

#undef lookup_impl

bool Fn::operator==(const Fn &other) const {
  return ret == other.ret && err == other.err && params == other.params;
}

bool Derive::operator==(const Derive &other) const {
  return ty == other.ty && underlying == other.underlying;
}

std::string Typebase::repr(type_id id) const {
  std::stringstream ss;

  switch (id.category()) {
  case type_id::cat::meta:
    return "!!!";
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

} // namespace types
} // namespace arcana
