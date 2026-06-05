#include <bit>
#include <cstdlib>
#include <getopt.h>

#include <iostream>
#include <string_view>

#include <chroma.h>
#include <mfile.h>
#include <sigil.h>

#include "arcana.h"
#include "generate.h"
#include "symbol.h"

#include "args.h"
#include "ast.h"

#include "reports.h"

int main(int argc, char **argv) {
  parse_args(argc, argv);
  validate_args();

  mfile content = read_file();

  const sigil::Tokens<arcana::Token> tokens = tokenize(content);

  if (stops == 1) {
    report::tokens(tokens);
    return 0;
  }

  const sigil::Ast<arcana::Node> ast = parse_ast(tokens);

  if (stops == 2) {
    report::ast(tokens, ast);
    return 0;
  }

  SymbolTable syms{4096, 16};
  arcana::pass::NamePass name_pass{tokens, ast, syms};

  name_pass.run();

  if (stops == 4) {
    report::symbols(syms);
    report::names(ast, syms, name_pass.overlay);
    return 0;
  }

  arcana::types::Typebase base{syms};

  arcana::pass::TypeDefPass type_def{tokens, ast, syms, base,
                                     name_pass.overlay};
  type_def.run();

  if (stops == 8) {
    report::types(tokens, ast, name_pass.overlay, base, type_def);
    return 0;
  }

  gen::llvm g(std::cout, tokens, ast, name_pass, type_def);

  if (stops == 16) {
    g.generate();

    return 0;
  }

  return 0;
}
