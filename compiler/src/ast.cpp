#include "ast.h"
#include "arcana.h"
#include "args.h"
#include <chroma.h>
#include <iostream>

sigil::Tokens<arcana::Token> tokenize(std::string_view content) {
  try {
    sigil::Tokens<arcana::Token> tokens{content, arcana::tokenizer};
    return tokens;
  } catch (sigil_tokens_error err) {

    switch (err.err) {
    case SIGIL_TOKENS_ERROR_MAP:
      std::cerr << "failed to map memory" << std::endl;
      break;
    case SIGIL_TOKENS_ERROR_OVERFLOW:
      std::cerr << "File to large to tokenize" << std::endl;
      break;
    case SIGIL_TOKENS_ERROR_INVALID:
      std::cerr << "Invalid token found" << std::endl;
      auto len = content.find_first_of("\n", err.pos);
      auto sub = content.substr(err.pos, len - 1);
      std::cerr << sub << std::endl;
      std::cerr << "^" << std::endl;

      exit(2);
      break;
    }
    exit(4);
  } catch (...) {
    std::cerr << chroma::red << "unknown error while tokenizing" << std::endl;
    exit(4);
  }
}

std::array<std::string_view, 3>
extract_line(std::string_view view, sigil::Tokens<arcana::Token>::Token token);

sigil::Ast<arcana::Node> parse_ast(const sigil::Tokens<arcana::Token> &tokens) {
  try {
    sigil::Ast<arcana::Node> ast{arcana::parser, tokens};
    return ast;
  } catch (sigil_parser_error err) {
    auto token = tokens[err.token];
    sigil_linemeta meta = tokens.linemeta(err.token);

    auto [pre, cur, post] = extract_line(tokens.content(), token);

    std::cerr << err.status << " invalid token found at " << meta.line << ":"
              << meta.column << std::endl;
    std::cerr << chroma::clear << pre << chroma::red << cur << chroma::clear
              << post << std::endl;

    std::string pad(pre.length(), ' ');

    std::cerr << pad << "^" << std::endl;
    exit(2);
  } catch (...) {
    std::cerr << chroma::red << "unknown error while tokenizing" << std::endl;
    exit(4);
  }
}

std::array<std::string_view, 3>
extract_line(std::string_view view, sigil::Tokens<arcana::Token>::Token token) {
  size_t offset = token.off;

  while (offset > 0) {
    if (view[offset - 1] == '\n') {
      break;
    }

    offset--;
  }

  size_t len = 0;

  while (token.off + token.len + len + 1 < view.length()) {
    if (view[token.off + token.len + len + 1] == '\n') {
      break;
    }

    len++;
  }

  return {
      view.substr(offset, token.off - offset),
      view.substr(token.off, token.len),
      view.substr(token.off + token.len, len),
  };
}
