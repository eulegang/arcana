#pragma once

#include <map>
#include <ostream>
#include <unordered_map>

#include "arcana.h"
#include "arcana/entries.h"
#include "arcana/pass.h"
#include "arcana/types.h"
#include "emitter.h"
#include "symbol.h"

namespace gen {

struct Unit {
  struct Overlays {
    const arcana::pass::NamePass::Overlay &names;
    const arcana::types::TypeDefPass::Overlay &types;
  };

  const arcana::Tokens &tokens;
  const arcana::Ast &ast;
  const Overlays overlays;
  const SymbolTable &symbols;
  const arcana::types::Typebase &types;
  const arcana::entry::Entries &entries;
};

struct GenComponent {
  lir::Emitter &emitter;
  Unit &unit;

  GenComponent(lir::Emitter &emitter, Unit &unit)
      : emitter{emitter}, unit{unit} {}

  virtual void generate() = 0;
};

struct EntryComponent : GenComponent {
  bool has_main();
  void gen_main();

  void generate() override;
  EntryComponent(lir::Emitter &emitter, Unit &unit)
      : GenComponent{emitter, unit} {}
};

struct TypesComponent : GenComponent {
  void generate() override;
  TypesComponent(lir::Emitter &emitter, Unit &unit)
      : GenComponent{emitter, unit} {}

private:
  void visit(sigil_node_id);
  void gen(sigil_node_id, arcana::types::type_id,
           const arcana::types::BitSet &);
  void gen(sigil_node_id, arcana::types::type_id,
           const arcana::types::Enumeration &);
  void gen(sigil_node_id, arcana::types::type_id,
           const arcana::types::Struct &);
  void gen(sigil_node_id, arcana::types::type_id, const arcana::types::Alias &);
};

struct EntriesComponent : GenComponent {
  void generate() override;
  EntriesComponent(lir::Emitter &emitter, Unit &unit)
      : GenComponent{emitter, unit} {}
};

struct ForeignComponent : GenComponent {
  void generate() override;
  ForeignComponent(lir::Emitter &emitter, Unit &unit)
      : GenComponent{emitter, unit} {}
};

struct FuncComponent : GenComponent {
  void generate() override;
  FuncComponent(lir::Emitter &emitter, Unit &unit)
      : GenComponent{emitter, unit} {}
};

struct Generator : GenComponent {
  void generate() override;
  Generator(lir::Emitter &emitter, Unit &unit) : GenComponent{emitter, unit} {}
};

std::string name_of(Unit &unit, uint16_t node);
std::string type_name(Unit &unit, arcana::types::type_id tid);
const char *prim_name(uint32_t id);

} // namespace gen
