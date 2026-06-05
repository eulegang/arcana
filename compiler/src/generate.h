#pragma once

#include <map>
#include <ostream>
#include <unordered_map>

#include "arcana.h"
#include "arcana/pass.h"
#include "arcana/types.h"
#include "symbol.h"

namespace gen {
struct generator {
  std::ostream &out;
  const arcana::Tokens &tokens;
  const arcana::Ast &ast;
  const arcana::pass::NamePass &names;
  const arcana::pass::TypeDefPass &types;

  generator(std::ostream &out, const arcana::Tokens &tokens,
            const arcana::Ast &ast, const arcana::pass::NamePass &names,
            const arcana::pass::TypeDefPass &types)

      : out{out}, tokens{tokens}, ast{ast}, names{names}, types{types} {}

  virtual void generate() = 0;
};

struct llvm : generator {
  llvm(std::ostream &out, const arcana::Tokens &tokens, const arcana::Ast &ast,

       const arcana::pass::NamePass &names,
       const arcana::pass::TypeDefPass &types)
      : generator{out, tokens, ast, names, types} {}

  void generate() override;

private:
  std::string name_of(uint16_t node);

  void gen_types();
  void gen_fns();
  void gen(uint16_t, arcana::types::type_id, arcana::types::BitSet &);
  void gen(uint16_t, arcana::types::type_id, arcana::types::Enumeration &);
  void gen(uint16_t, arcana::types::type_id, arcana::types::Struct &);
  void gen(uint16_t, arcana::types::type_id, arcana::types::Alias &);

  void gen_func(uint16_t);

  std::string fn_name(arcana::types::Fn);
  std::string type_name(arcana::types::type_id);

  bool is_definable(arcana::types::type_id);

  std::vector<std::pair<uint16_t, arcana::types::type_id>> pending;

  std::map<arcana::types::type_id, std::string> alloc_names;
};
} // namespace gen
