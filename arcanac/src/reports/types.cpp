#include "pass/types.h"
#include "../reports.h"
#include "symbol.h"
#include <chroma.h>
#include <iostream>
#include <iterator>
#include <ranges>

struct ctx {
  std::ostream *out;
  const SymbolTable &table;
  const arcana::pass::TypeDefPass::Overlay &overlay;
};

std::ostream &operator<<(std::ostream &out, arcana::pass::type_id tid) {
  if (!tid) {
    return out << chroma::purple << "null";
  }

  out << chroma::clear << "(" << chroma::purple;
  switch (tid.category()) {
  case arcana::pass::type_id::cat::bs:
    out << "bitset";
    break;
  case arcana::pass::type_id::cat::en:
    out << "enum";
    break;
  case arcana::pass::type_id::cat::st:
    out << "struct";
    break;
  case arcana::pass::type_id::cat::prim:
    out << "primitive";
    break;
  case arcana::pass::type_id::cat::ref:
    out << "ref";
    break;
  case arcana::pass::type_id::cat::derive:
    out << "derive";
    break;
  case arcana::pass::type_id::cat::fn:
    out << "fn";
    break;
  case arcana::pass::type_id::cat::alias:
    out << "alias";
    break;

  default:
    out << chroma::red << "oh no!";
    break;
  }

  return out << chroma::clear << ")" << chroma::yellow << tid.id();
}

void types_dump_nodes(uint16_t id, sigil::Ast<arcana::Node>::Node node, void *,
                      size_t level, ctx *ctx) {

  auto &out = *ctx->out;

  out << std::string(2 * level, ' ');
  out << node.type;

  const arcana::pass::type_id *tid = ctx->overlay.resolve(id);
  if (tid) {
    out << " " << *tid;
  }

  out << chroma::clear << std::endl;
}

void report::types(const arcana::Tokens &, const arcana::Ast &ast,
                   const arcana::pass::NamePass::Overlay &scopes,
                   const arcana::pass::TypeDefPass &pass) {

  std::ostream *out = &std::cout;
  ctx ctx = {
      .out = out,
      .table = pass.table,
      .overlay = pass.type_overlay,
  };

  ast.visit(&ctx, types_dump_nodes);

  std::cout << chroma::purple << "summary" << std::endl;

  uint16_t id = 0;
  std::cout << chroma::purple << "  bitsets" << std::endl;
  for (const auto &bitset : pass.bitsets) {
    arcana::pass::type_id tid(arcana::pass::type_id::cat::bs, id++);

    auto name = scopes.resolve(bitset.node);

    std::string fullname = resolve(scopes, pass.table, *name, "::");

    std::cout << "    " << chroma::green << fullname << chroma::clear << " ("
              << chroma::yellow << bitset.size << chroma::clear << ") " << tid
              << std::endl;

    for (const auto &c : bitset.cases) {
      auto x = pass.table.resolve(c.sym);

      std::cout << "      " << chroma::cyan << x << chroma::clear << " = "
                << chroma::yellow << c.bit << std::endl;
    }
  }

  id = 0;
  std::cout << chroma::purple << "  enums" << std::endl;
  for (const auto &en : pass.enums) {
    arcana::pass::type_id tid(arcana::pass::type_id::cat::en, id++);
    auto name = scopes.resolve(en.node);

    std::string fullname = resolve(scopes, pass.table, *name, "::");

    std::cout << "    " << chroma::green << fullname << chroma::clear << " ("
              << chroma::yellow << en.size << chroma::clear << ") " << tid
              << std::endl;

    for (const auto &c : en.cases) {
      auto x = pass.table.resolve(c.sym);

      std::cout << "      " << chroma::cyan << x << chroma::clear << " = "
                << chroma::yellow << c.pattern << std::endl;
    }
  }

  id = 0;
  std::cout << chroma::purple << "  structs" << std::endl;
  for (const auto &st : pass.structs) {
    arcana::pass::type_id tid(arcana::pass::type_id::cat::st, id++);
    auto name = scopes.resolve(st.node);
    std::string fullname = resolve(scopes, pass.table, *name, "::");

    std::cout << "    " << chroma::green << fullname << " " << tid << std::endl;

    for (const auto &c : st.fields) {
      auto x = pass.table.resolve(c.sym);

      std::cout << "      " << chroma::cyan << x << chroma::clear << ": "
                << c.ty << std::endl;
    }
  }

  id = 0;
  std::cout << chroma::purple << "  refs" << std::endl;
  std::string ref_repr;
  for (const auto &ref : pass.refs) {
    ref_repr.clear();
    arcana::pass::type_id tid(arcana::pass::type_id::cat::ref, id++);
    auto name = scopes.resolve(ref.node);
    std::string fullname = resolve(scopes, pass.table, *name, "::");

    std::cout << "    " << chroma::green << fullname << " " << tid << std::endl;

    for (const auto &c : ref.syms) {
      if (c == 0)
        continue;

      auto x = pass.table.resolve(c);
      ref_repr += x;
      ref_repr += "::";
    }

    if (ref_repr.size() > 0)
      ref_repr.resize(ref_repr.size() - 2);
    std::cout << "      " << chroma::cyan << ref_repr << std::endl;
  }

  id = 0;
  std::cout << chroma::purple << "  derives" << std::endl;
  for (const auto &derive : pass.derives) {
    arcana::pass::type_id tid(arcana::pass::type_id::cat::derive, id++);

    std::cout << "    " << chroma::purple;

    switch (derive.ty) {
    case arcana::pass::Derive::Type::Pointer:
      std::cout << "* ";
      break;
    case arcana::pass::Derive::Type::Slice:
      std::cout << "[] ";
      break;
    }

    std::cout << derive.underlying << " " << tid << std::endl;
  }

  id = 0;
  std::cout << chroma::purple << "  fns" << std::endl;
  for (const auto &fn : pass.fns) {
    arcana::pass::type_id tid(arcana::pass::type_id::cat::fn, id++);

    std::cout << "    " << chroma::clear << "(";

    for (const auto &param : fn.params) {
      std::cout << param << ", ";
    }

    std::cout << chroma::clear << ") -> " << fn.err << "!" << fn.ret
              << std::endl;
  }

  id = 0;
  std::cout << chroma::purple << "  aliases" << std::endl;
  for (const auto &alias : pass.aliases) {
    arcana::pass::type_id tid(arcana::pass::type_id::cat::alias, id++);

    std::cout << "    " << alias << " " << tid << std::endl;
  }

  std::cout << chroma::clear;
}
