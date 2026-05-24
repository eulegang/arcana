#pragma once

#include "arcana.h"
#include <sigil.h>
#include <string_view>
#include <vector>

namespace testing {
template <typename T>
std::vector<std::string_view> sigil_slices(const sigil::Tokens<T> &tokens) {
  std::vector<std::string_view> res;
  size_t len = tokens.length();

  for (size_t i = 0; i < len; i++) {
    res.push_back(tokens.content(i));
  }

  return res;
}

template <typename T>
std::vector<T> sigil_token_types(const sigil::Tokens<T> &tokens) {
  std::vector<T> res;

  size_t len = tokens.length();

  for (size_t i = 0; i < len; i++) {
    res.push_back(tokens[i].type);
  }

  return res;
}
} // namespace testing
