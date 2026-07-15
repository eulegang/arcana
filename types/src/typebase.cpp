#include "arcana/typebase.h"
#include "arcana/type_id.h"

using namespace arcana::types;

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
generate(Fn, fn, fns);

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

type_id Typebase::member(type_id base, symbol sym) const {
  std::string_view name = table.resolve(sym);

  switch (base.category()) {
  case type_id::cat::derive: {

    auto derive = lookup<Derive>(base);
    switch (derive.ty) {
    case Derive::Type::Pointer:
      return type_id::poison;

    case Derive::Type::Slice:
      if (name == "len") {
        return type_id(type_id::cat::prim, 8);
      } else if (name == "data") {
        return derive.underlying;
      }
      return type_id::poison;
      break;
    }

  } break;

  default:
    return type_id::poison;
  }
  return type_id::poison;
}
