#include "arcana/types.h"
#include "../reports.h"
#include "symbol.h"
#include <chroma.h>
#include <cstdint>
#include <format>
#include <iomanip>
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
    if (!verbose) {
      out << " " << *tid;
    } else {
      out << " " << ctx->base.repr(*tid);
    }
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

  if (!verbose) {
    auto padd = 12;
    for (uint16_t id = 0; id < base.bitsets.size(); id++) {
      arcana::types::type_id tid(arcana::types::type_id::cat::bs, id);
      std::cout << std::left << std::setw(padd) << tid << ": " << base.repr(tid)
                << std::endl;
    }

    for (uint16_t id = 0; id < base.enums.size(); id++) {
      arcana::types::type_id tid(arcana::types::type_id::cat::en, id);
      std::cout << std::left << std::setw(padd) << tid << ": " << base.repr(tid)
                << std::endl;
    }

    for (uint16_t id = 0; id < base.structs.size(); id++) {
      arcana::types::type_id tid(arcana::types::type_id::cat::st, id);
      std::cout << std::left << std::setw(padd) << tid << ": " << base.repr(tid)
                << std::endl;
    }

    for (uint16_t id = 0; id < base.derives.size(); id++) {
      arcana::types::type_id tid(arcana::types::type_id::cat::derive, id);
      std::cout << std::left << std::setw(padd) << tid << ": " << base.repr(tid)
                << std::endl;
    }

    for (uint16_t id = 0; id < base.fns.size(); id++) {
      arcana::types::type_id tid(arcana::types::type_id::cat::fn, id);
      std::cout << std::left << std::setw(padd) << tid << ": " << base.repr(tid)
                << std::endl;
    }

    for (uint16_t id = 0; id < base.aliases.size(); id++) {
      arcana::types::type_id tid(arcana::types::type_id::cat::alias, id);
      std::cout << std::left << std::setw(padd) << tid << ": " << base.repr(tid)
                << std::endl;
    }
  }

  std::cout << std::endl;

  ast.visit(&ctx, types_dump_nodes);

  std::cout << chroma::clear;
}
