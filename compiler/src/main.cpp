#include <bit>
#include <cstdlib>
#include <getopt.h>

#include <iostream>
#include <sstream>
#include <string_view>

#include <chroma.h>
#include <mfile.h>
#include <sigil.h>

#include "arcana.h"
#include "arcana/entries.h"
#include "arcana/pass.h"
#include "assemble.h"
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

  arcana::Diagnostics diagnostics;

  SymbolTable syms{4096, 16};
  arcana::pass::NamePass name_pass{tokens, ast, syms, diagnostics};
  name_pass.run();

  if (diagnostics.has_errors()) {
    report::diagnostics(path, tokens, diagnostics);
    return 2;
  }

  if (stops == 4) {
    if (diagnostics) {
      report::diagnostics(path, tokens, diagnostics);
    }

    report::symbols(syms);
    report::names(ast, syms, name_pass.overlay);
    return 0;
  }

  arcana::types::Typebase base{syms};
  arcana::entry::Entries entries;

  arcana::pass::TypeDefPass type_def{
      tokens, ast, syms, base, name_pass.overlay, diagnostics};
  type_def.run();

  arcana::pass::EntryPass entry_pass{tokens, ast, entries};
  entry_pass.run();

  if (diagnostics.has_errors()) {
    report::diagnostics(path, tokens, diagnostics);
    return 2;
  }

  if (stops == 8) {
    if (diagnostics) {
      report::diagnostics(path, tokens, diagnostics);
    }

    report::types(tokens, ast, name_pass.overlay, base, type_def);
    return 0;
  }

  if (stops == 16) {
    if (diagnostics) {
      report::diagnostics(path, tokens, diagnostics);
    }

    gen::llvm g(std::cout, tokens, ast, name_pass, type_def, entries);
    g.generate();

    return 0;
  }

  std::stringstream buf;
  gen::llvm g(buf, tokens, ast, name_pass, type_def, entries);
  g.generate();

  assemble(buf.str(), output);

  if (diagnostics) {
    report::diagnostics(path, tokens, diagnostics);
  }

  return 0;
}
