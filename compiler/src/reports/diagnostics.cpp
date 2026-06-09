#include "../reports.h"
#include <chroma.h>
#include <iostream>

using namespace arcana;

namespace report {

std::array<std::string_view, 3>
split_content(std::string_view view, Tokens::Token start, Tokens::Token end) {
  size_t offset = start.off;

  while (offset > 0) {
    if (view[offset - 1] == '\n') {
      break;
    }

    offset--;
  }

  size_t len = 0;

  while (end.off + end.len + len + 1 < view.length()) {
    if (view[end.off + end.len + len + 1] == '\n') {
      break;
    }

    len++;
  }

  return {
      view.substr(offset, start.off - offset),
      view.substr(start.off, (end.off - start.off) + end.len),
      view.substr(end.off + end.len, len),
  };
}

void diagnostics(std::string_view filename, const Tokens &tokens,
                 Diagnostics &diagnostics) {
  auto content = tokens.content();
  for (const auto &diag : diagnostics) {
    const bool error = diag.flags & 0x01;
    const auto color = error ? chroma::red : chroma::yellow;
    const auto type = error ? "ERR " : "WARN";
    Tokens::Token start = tokens[diag.span.start];
    Tokens::Token end = tokens[diag.span.end];

    sigil_linemeta meta = tokens.linemeta(diag.span.start);

    std::cerr << color << "[" << type << "] " << chroma::clear << filename
              << ":" << meta.line << ":" << meta.column << ": " << diag.message
              << std::endl;

    auto [pre, cur, post] = split_content(content, start, end);

    std::cerr << chroma::clear << pre << color << cur << chroma::clear << post
              << std::endl;
  }
}

} // namespace report
