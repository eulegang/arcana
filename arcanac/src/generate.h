#pragma once

#include <ostream>

#include "arcana.h"
#include "arcana/pass.h"
#include "arcana/types.h"
#include "symbol.h"

namespace gen {
struct generator {
  const arcana::Tokens &tokens;
  const arcana::Ast &ast;
  const arcana::pass::NamePass &names;
  const arcana::pass::TypeDefPass &types;

  generator(const arcana::Tokens &tokens, const arcana::Ast &ast,
            const arcana::pass::NamePass &names,
            const arcana::pass::TypeDefPass &types)

      : tokens{tokens}, ast{ast}, names{names}, types{types} {}

  virtual void generate(std::ostream &out) = 0;
};

struct llvm : generator {
  llvm(const arcana::Tokens &tokens, const arcana::Ast &ast,

       const arcana::pass::NamePass &names,
       const arcana::pass::TypeDefPass &types)
      : generator{tokens, ast, names, types} {}

  void generate(std::ostream &out) override;

private:
  std::string name_of(uint16_t node);
  void generate_bitsets(std::ostream &out);
  void generate_enumerations(std::ostream &out);
};
} // namespace gen
