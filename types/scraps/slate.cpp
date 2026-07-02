#include "slate.h"
#include "gmock/gmock.h"
#include <algorithm>
#include <sigil.h>
#include <stack>
#include <stdexcept>

using namespace arcana::types;

namespace arcana::types {
using InferCache =
    std::map<sigil_node_id, std::variant<sigil_node_id, type_id>>;

bool strong_check(const InferCache &cache, sigil_node_id id) {
  if (auto it = cache.find(id); it != cache.end()) {
    if (auto tid = std::get_if<type_id>(&std::get<1>(*it))) {
      if (*tid != type_id()) {
        return true;
      }
    }
  }

  return false;
}
} // namespace arcana::types

void TypeSlate::push(sigil_node_id id) { _unknowns.push_back({id}); }

void TypeSlate::link(sigil_node_id dst, sigil_node_id src) {
  _links.push_back({dst, src});
}

void TypeSlate::set(sigil_node_id dst, type_id tid) {
  _facts.push_back({dst, tid});
}

void TypeSlate::hint(sigil_node_id dst, type_id tid) {
  _hints.push_back({dst, tid});
}

void TypeSlate::compress() {
  const type_id poison{type_id::cat::meta, 1};
  InferCache cache;

  for (const auto id : _unknowns) {
    cache[id] = type_id();
  }

  for (const auto &[id, tid] : _facts) {
    cache[id] = tid;
  }

  for (const auto &[a, b] : _links) {
    bool a_strong = strong_check(cache, a);
    bool b_strong = strong_check(cache, b);

    if (!a_strong && !b_strong) {
      cache[a] = b;
    } else if (a_strong) {
      const auto tid = std::get<type_id>(cache[a]);

      for (const auto linked : linked(a)) {
        cache[linked] = tid;
      }
    } else if (b_strong) {
      const auto tid = std::get<type_id>(cache[b]);
      for (const auto linked : linked(b)) {
        cache[linked] = tid;
      }
    } else {
      auto a_ty = std::get<type_id>(cache[a]);
      auto b_ty = std::get<type_id>(cache[b]);

      if (a_ty != b_ty) {
        cache[a] = poison;
        cache[b] = poison;
      }
    }
  }

  for (const auto [id, tid] : _hints) {
    if (!strong_check(cache, id)) {
      cache[id] = tid;
      for (const auto linked : linked(id)) {
        cache[linked] = tid;
      }
    }
  }

  _facts.clear();
  for (const auto &[key, value] : cache) {
    if (auto v = std::get_if<type_id>(&value)) {
      _facts.push_back({key, *v});
    } else {
      auto id = std::get<sigil_node_id>(value);
      if (auto tid = std::get_if<type_id>(&cache[id])) {
        _facts.push_back({key, *tid});
      }
    }
  }
}

std::vector<sigil_node_id> TypeSlate::linked(sigil_node_id root) {
  std::vector<sigil_node_id> results;

  std::stack<sigil_node_id> fringe;
  fringe.push(root);

  while (!fringe.empty()) {
    auto cur = fringe.top();
    fringe.pop();

    for (const auto &[a, b] : _links) {
      if (a == cur || b == cur) {
        auto next = cur == a ? b : a;
        if (std::find(results.begin(), results.end(), cur) != results.end()) {
          fringe.push(next);
        }

        results.push_back(next);
      }
    }
  }
  return std::vector(results);
}
