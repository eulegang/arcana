#include <bit>
#include <cstdlib>
#include <getopt.h>

#include <iostream>
#include <string_view>

#include <chroma.h>
#include <mfile.h>
#include <sigil.h>

#include "arcana.h"
#include "args.h"
#include "ast.h"
#include "pass/name.h"
#include "pass/types.h"
#include "symbol.h"
#include "symbols.h"
#include "tokenize.h"
#include "types.h"

int main(int argc, char **argv) {
  parse_args(argc, argv);
  validate_args();

  mfile content = read_file();

  const sigil::Tokens<arcana::Token> tokens = tokenize(content);

  if (stops == 1) {
    report_tokens(tokens);
    return 0;
  }

  const sigil::Ast<arcana::Node> ast = parse_ast(tokens);

  if (stops == 2) {
    report_ast(tokens, ast);
    return 0;
  }

  SymbolTable syms{4096, 16};
  arcana::pass::NamePass name_pass{syms};

  name_pass.run(tokens, ast);

  if (stops == 4) {
    report_symbols(syms);
    return 0;
  }

  arcana::pass::TypeDefPass type_def{syms};
  type_def.run(tokens, ast);

  if (stops == 8) {
    report_types(tokens, ast, name_pass.overlay, type_def);
    return 0;
  }

  return 0;
}
