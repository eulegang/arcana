#include "../reports.h"
#include "symbol.h"
#include <chroma.h>
#include <iostream>
#include <iterator>
#include <ranges>

struct ctx {
  std::ostream *out;
  const SymbolTable &table;
  const arcana::types::Typebase &base;
  const arcana::pass::TypeDefPass::Overlay &overlay;
};

std::ostream &operator<<(std::ostream &out, arcana::types::type_id tid) {
  if (!tid) {
    return out << chroma::purple << "null";
  }

  out << chroma::clear << "(" << chroma::purple;
  switch (tid.category()) {
  case arcana::types::type_id::cat::bs:
    out << "bitset";
    break;
  case arcana::types::type_id::cat::en:
    out << "enum";
    break;
  case arcana::types::type_id::cat::st:
    out << "struct";
    break;
  case arcana::types::type_id::cat::prim:
    out << "primitive";
    break;
  case arcana::types::type_id::cat::ref:
    out << "ref";
    break;
  case arcana::types::type_id::cat::derive:
    out << "derive";
    break;
  case arcana::types::type_id::cat::fn:
    out << "fn";
    break;
  case arcana::types::type_id::cat::alias:
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

  const arcana::types::type_id *tid = ctx->overlay.resolve(id);
  if (tid) {
    out << " " << *tid;
  }

  out << chroma::clear << std::endl;
}

void report::types(const arcana::Tokens &, const arcana::Ast &ast,
                   const arcana::pass::NamePass::Overlay &scopes,
                   const arcana::types::Typebase &base,
                   const arcana::pass::TypeDefPass &pass) {

  std::ostream *out = &std::cout;
  ctx ctx = {
      .out = out,
      .table = pass.table,
      .base = base,
      .overlay = pass.overlay,
  };

  ast.visit(&ctx, types_dump_nodes);

  auto lookup = [&scopes, &pass](arcana::types::type_id tid) -> std::string {
    uint16_t node = 0xFFFF;
    for (const auto &[n, t] : pass.ids) {
      if (t == tid) {
        node = n;
        break;
      }
    }

    if (node == 0xFFFF) {
      return "!!!";
    }

    auto name = scopes.resolve(node);

    return resolve(scopes, pass.table, *name, "::");
  };

  std::cout << chroma::purple << "summary" << std::endl;

  uint16_t id = 0;
  std::cout << chroma::purple << "  bitsets" << std::endl;
  for (const auto &bitset : base.bitsets) {
    arcana::types::type_id tid(arcana::types::type_id::cat::bs, id++);
    std::string fullname = lookup(tid);

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
  for (const auto &en : base.enums) {
    arcana::types::type_id tid(arcana::types::type_id::cat::en, id++);
    std::string fullname = lookup(tid);

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
  for (const auto &st : base.structs) {
    arcana::types::type_id tid(arcana::types::type_id::cat::st, id++);
    std::string fullname = lookup(tid);

    std::cout << "    " << chroma::green << fullname << " " << tid << std::endl;

    for (const auto &c : st.fields) {
      auto x = pass.table.resolve(c.sym);

      std::cout << "      " << chroma::cyan << x << chroma::clear << ": "
                << c.ty << std::endl;
    }
  }

  id = 0;
  std::cout << chroma::purple << "  refs" << std::endl;
  std::string buffer;
  for (const auto &ref : base.refs) {
    buffer.clear();
    arcana::types::type_id tid(arcana::types::type_id::cat::ref, id++);
    auto name = scopes.resolve(ref.node);
    std::string fullname = resolve(scopes, pass.table, *name, "::");

    std::cout << "    " << chroma::green << fullname << " " << tid << std::endl;

    for (const auto &c : ref.syms) {
      if (c == 0)
        continue;

      auto x = pass.table.resolve(c);
      buffer += x;
      buffer += "::";
    }

    if (buffer.size() > 0)
      buffer.resize(buffer.size() - 2);
    std::cout << "      " << chroma::cyan << buffer << std::endl;
  }

  id = 0;
  std::cout << chroma::purple << "  derives" << std::endl;
  for (const auto &derive : base.derives) {
    arcana::types::type_id tid(arcana::types::type_id::cat::derive, id++);

    std::cout << "    " << chroma::purple;

    switch (derive.ty) {
    case arcana::types::Derive::Type::Pointer:
      std::cout << "* ";
      break;
    case arcana::types::Derive::Type::Slice:
      std::cout << "[] ";
      break;
    }

    std::cout << derive.underlying << " " << tid << std::endl;
  }

  id = 0;
  std::cout << chroma::purple << "  fns" << std::endl;
  for (const auto &fn : base.fns) {
    arcana::types::type_id tid(arcana::types::type_id::cat::fn, id++);

    std::cout << "    " << chroma::clear << "(";
    for (const auto &param : fn.params) {
      std::cout << param << ", ";
    }

    std::cout << chroma::clear << ") -> " << fn.err << "!" << fn.ret
              << std::endl;
  }

  id = 0;
  std::cout << chroma::purple << "  aliases" << std::endl;
  for (const auto &alias : base.aliases) {
    arcana::types::type_id tid(arcana::types::type_id::cat::alias, id++);

    std::cout << "    " << alias.id << " " << tid << std::endl;
  }

  std::cout << chroma::clear;
}
