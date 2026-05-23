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
  const arcana::pass::NamePass::Output names;
  const arcana::pass::TypeDefPass::Output types;

  generator(const arcana::Tokens &tokens, const arcana::Ast &ast,
            arcana::pass::NamePass::Output names,
            arcana::pass::TypeDefPass::Output types)

      : tokens{tokens}, ast{ast}, names{names}, types{types} {}

  virtual void generate(std::ostream &out) = 0;
};

struct llvm : generator {
  llvm(const arcana::Tokens &tokens, const arcana::Ast &ast,

       const arcana::pass::NamePass::Output names,
       const arcana::pass::TypeDefPass::Output types)
      : generator{tokens, ast, names, types} {}

  void generate(std::ostream &out) override;
};
} // namespace gen
