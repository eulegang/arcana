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
#include "symbol.h"
#include "tokenize.h"

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
  arcana::pass::NamePass pass{syms};

  pass.run(tokens, ast);

  for (const auto &sym : syms) {
    std::cout << sym << ":\t" << syms.resolve(sym) << std::endl;
  }

  return 0;
}
