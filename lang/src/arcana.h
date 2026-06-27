#pragma once

#include "sigil.h"
#include <type_traits>
#include <vector>

#include "arcana_nodes.h"
#include "arcana_tokens.h"

namespace arcana {

class Diagnostics {
  struct Diag {
    std::string message;
    sigil_span span;
    uint16_t flags;
  };

  std::vector<Diag> diagnostics;

public:
  Diagnostics() : diagnostics{} {}

  operator bool() const;
  bool has_errors() const;
  bool has_warnings() const;

  void add_warning(std::string, sigil_span);
  void add_error(std::string, sigil_span);

  decltype(diagnostics.begin()) begin() { return diagnostics.begin(); }
  decltype(diagnostics.end()) end() { return diagnostics.end(); }
};

ssize_t tokenizer(size_t, sigil_slice, Token *);

std::ostream &operator<<(std::ostream &, const arcana::Token &);

const uint16_t ACCESS_OPAQUE = 1;

enum class Perc : size_t {
  LOWEST,

  HIGHEST,
};

extern sigil_parser *parser;

using Tokens = sigil::Tokens<arcana::Token>;
using Ast = sigil::Ast<arcana::Node>;
// using Pass = sigil::Pass<arcana::Token, arcana::Node>;

struct Pass : public sigil::Pass<arcana::Token, arcana::Node> {
  enum class Branch {
    /// Iterate child then next
    Nest,

    /// Iterate next then child
    Defer,

    /// iterate child
    Child,

    /// iterate next
    Next,

    /// stop iteration
    Terminate,
  };

  Pass(const Tokens &tokens, const Ast &ast)
      : sigil::Pass<arcana::Token, arcana::Node>::Pass{tokens, ast} {}

  void run();
  void iterate(sigil_node_id id);

  virtual Branch visit(sigil_node_id id) = 0;
};

} // namespace arcana
