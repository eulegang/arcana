#pragma once

#include "../arcana.h"
#include "symbol.h"
#include <cstdint>
#include <sigil.h>
#include <vector>

namespace arcana {
namespace pass {

struct type_id {
  enum class cat : uint16_t {
    bs = 1,
    en = 2,
  };

  type_id(cat category, uint16_t id) {
    if (0xE000 & id) {
      throw std::runtime_error("type id overflow");
    }

    payload = ((uint16_t)category << 13) | id;
  }

  cat category() const { return (cat)((0xE000 & payload) >> 13); }
  uint16_t id() const { return (~0xE000 & payload); }

private:
  uint16_t payload;
};

struct BitSet {
  struct Case {
    symbol sym;
    uint16_t bit;
  };

  uint16_t node;
  uint16_t size;
  std::vector<Case> cases;

  BitSet() : node{}, size{}, cases{} {}
};

struct Enumeration {
  struct Case {
    symbol sym;
    uint64_t pattern;
  };

  uint16_t node;
  uint16_t size;

  std::vector<Case> cases;
  Enumeration() : node{}, size{}, cases{} {}
};

struct TypeDefPass : Pass {
  using Overlay = sigil::Overlay<type_id>;

  SymbolTable &table;
  std::vector<BitSet> bitsets;
  std::vector<Enumeration> enums;
  Overlay type_overlay;

  TypeDefPass(SymbolTable &table) : table{table}, bitsets{} {}

  void run(const Tokens &, const Ast &) override;
  void visit(const Tokens &, const Ast &, uint16_t cur);
  void visit_bs(const Tokens &, const Ast &, uint16_t cur, BitSet &);
  void visit_en(const Tokens &, const Ast &, uint16_t cur, Enumeration &);
};

} // namespace pass
} // namespace arcana
