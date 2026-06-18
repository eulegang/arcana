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
    const arcana::pass::TypeDefPass::Overlay &types;
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

struct Generator : GenComponent {
  void generate() override;
  Generator(lir::Emitter &emitter, Unit &unit) : GenComponent{emitter, unit} {}
};

std::string name_of(Unit &unit, uint16_t node);
std::string type_name(Unit &unit, arcana::types::type_id tid);
const char *prim_name(uint32_t id);

struct generator {
  std::ostream &out;
  lir::Emitter emitter;
  const arcana::Tokens &tokens;
  const arcana::Ast &ast;
  const arcana::pass::NamePass &names;
  const arcana::pass::TypeDefPass &types;
  const arcana::entry::Entries &entries;

  generator(std::ostream &out, const arcana::Tokens &tokens,
            const arcana::Ast &ast, const arcana::pass::NamePass &names,
            const arcana::pass::TypeDefPass &types,
            const arcana::entry::Entries &entries)

      : out{out}, emitter{out}, tokens{tokens}, ast{ast}, names{names},
        types{types}, entries{entries} {}

  virtual void generate() = 0;
};

struct llvm : generator {
  llvm(std::ostream &out, const arcana::Tokens &tokens, const arcana::Ast &ast,
       const arcana::pass::NamePass &names,
       const arcana::pass::TypeDefPass &types,
       const arcana::entry::Entries &entries)
      : generator{out, tokens, ast, names, types, entries} {}

  void generate() override;

  std::string name_of(uint16_t node);

  bool has_main();
  void gen_main();

  void gen_types();
  void gen_fns();
  void gen_foreigns();
  void gen_constvars();

  void gen(uint16_t, arcana::types::type_id, arcana::types::BitSet &);
  void gen(uint16_t, arcana::types::type_id, arcana::types::Enumeration &);
  void gen(uint16_t, arcana::types::type_id, arcana::types::Struct &);
  void gen(uint16_t, arcana::types::type_id, arcana::types::Alias &);

  void gen_func(uint16_t);
  void gen_constvar(uint16_t);
  void gen_foreign(sigil_node_id);

  std::string fn_name(arcana::types::Fn);
  std::string type_name(arcana::types::type_id);

  bool is_definable(arcana::types::type_id);

  std::vector<std::pair<uint16_t, arcana::types::type_id>> pending;

  std::map<arcana::types::type_id, std::string> alloc_names;
};
} // namespace gen
