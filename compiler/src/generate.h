#pragma once

#include <map>
#include <ostream>
#include <unordered_map>

#include "arcana.h"
#include "arcana/entries.h"
#include "arcana/pass.h"
#include "arcana/types.h"
#include "symbol.h"

namespace gen {

struct Reg {
  std::string name;

  Reg(uint16_t reg) : name{std::format("%{}", reg)} {}
  Reg(std::string reg) : name{std::format("%{}", reg)} {}
  Reg(const char *reg) : name{std::format("%{}", reg)} {}
};

struct Global {
  std::string name;

  Global(std::string reg) : name{std::format("@{}", reg)} {}
  Global(const char *reg) : name{std::format("@{}", reg)} {}
};

struct Lit {
  std::string name;

  Lit(uint16_t reg) : name{std::format("{}", reg)} {}
  Lit(std::string reg) : name{std::format("{}", reg)} {}
};

template <typename T> struct Typed {
  std::string type;
  T value;
};

struct Declaration {
  std::string ret;
  std::string name;
  std::vector<std::string> args;
};

struct Definition {
  std::string ret;
  std::string name;
  std::vector<Typed<Reg>> args;

  class Emitter {
    std::ostream &out;
    uint16_t cur_inst;

  public:
    Emitter(std::ostream &out) : out{out}, cur_inst{1} {}
    ~Emitter();

    Reg stack(std::string type);
    Reg gep(std::string type, Reg ptr, uint32_t field);
    void store(Reg dst, Typed<Reg> src);
    Reg load(Typed<Reg> reg);
    Reg call(Typed<Global> func, std::vector<Typed<Reg>> args);
    void ret(Typed<Reg> ret);
  };
};

struct Emitter {
  std::ostream &out;

  Emitter(std::ostream &out) : out{out} {}

  Definition::Emitter define(const Definition &def);
  void declare(const Declaration &declare);
};

struct generator {
  std::ostream &out;
  Emitter emitter;
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

  void gen(uint16_t, arcana::types::type_id, arcana::types::BitSet &);
  void gen(uint16_t, arcana::types::type_id, arcana::types::Enumeration &);
  void gen(uint16_t, arcana::types::type_id, arcana::types::Struct &);
  void gen(uint16_t, arcana::types::type_id, arcana::types::Alias &);

  void gen_func(uint16_t);
  void gen_foreign(sigil_node_id);

  std::string fn_name(arcana::types::Fn);
  std::string type_name(arcana::types::type_id);

  bool is_definable(arcana::types::type_id);

  std::vector<std::pair<uint16_t, arcana::types::type_id>> pending;

  std::map<arcana::types::type_id, std::string> alloc_names;
};
} // namespace gen
//
std::ostream &operator<<(std::ostream &, const gen::Reg &);
std::ostream &operator<<(std::ostream &, const gen::Global &);
std::ostream &operator<<(std::ostream &, const gen::Lit &);

template <typename T>
std::ostream &operator<<(std::ostream &out, const gen::Typed<T> &ty) {
  return out << ty.type << " " << ty.value;
}
