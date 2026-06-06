#include "arcana/types.h"
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
                   const arcana::pass::NamePass::Overlay &,
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

  std::cout << chroma::purple << "summary" << std::endl;

  uint16_t id = 0;
  std::cout << chroma::purple << "  bitsets" << std::endl;
  for (const auto &bitset : base.bitsets) {
    arcana::types::type_id tid(arcana::types::type_id::cat::bs, id++);

    std::cout << "    size: " << chroma::yellow << bitset.size << chroma::clear
              << tid << std::endl;

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

    std::cout << "    size: " << chroma::yellow << en.size << chroma::clear
              << tid << std::endl;

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

    std::cout << "    " << tid << std::endl;

    for (const auto &c : st.fields) {
      auto x = pass.table.resolve(c.sym);

      std::cout << "      " << chroma::cyan << x << chroma::clear << ": "
                << c.ty << std::endl;
    }
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

    std::cout << chroma::clear << ") -> ";

    if (fn.err) {
      std::cout << fn.err << "!";

      if (fn.ret) {
        std::cout << fn.ret;
      } else {
        std::cout << arcana::types::type_id(arcana::types::type_id::cat::prim,
                                            0);
      }
    } else {
      if (fn.ret) {
        std::cout << fn.ret;
      } else {
        std::cout << arcana::types::type_id(arcana::types::type_id::cat::prim,
                                            0);
      }
    }
    std::cout << std::endl;
  }

  id = 0;
  std::cout << chroma::purple << "  aliases" << std::endl;
  for (const auto &alias : base.aliases) {
    arcana::types::type_id tid(arcana::types::type_id::cat::alias, id++);

    std::cout << "    " << alias.id << " " << tid << std::endl;
  }

  std::cout << chroma::clear;
}
