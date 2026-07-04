
#include "arcana/type_sync.h"
#include <algorithm>
#include <map>
#include <sigil.h>
#include <stack>
#include <variant>

using namespace arcana::types;

struct InferCache {
  using Key = sigil_node_id;
  using Value = std::variant<sigil_node_id, type_id>;

  std::map<Key, Value> mapping;

  Value &operator[](Key key) { return mapping[key]; }

  bool strong_check(sigil_node_id id) const {
    if (auto it = mapping.find(id); it != mapping.end()) {
      if (auto tid = std::get_if<type_id>(&std::get<1>(*it))) {
        if (*tid != type_id()) {
          return true;
        }
      }
    }

    return false;
  }

  auto begin() { return mapping.begin(); }
  auto end() { return mapping.end(); }
};

void TypeSync::push(sigil_node_id id) { _unknowns.push_back({id}); }

void TypeSync::link(sigil_node_id dst, sigil_node_id src) {
  _links.push_back({dst, src});
}

void TypeSync::set(sigil_node_id dst, type_id tid) {
  _facts.push_back({dst, tid});
}

void TypeSync::hint(sigil_node_id dst, type_id tid) {
  _hints.push_back({dst, tid});
}

void TypeSync::compress() {
  InferCache cache;

  for (const auto id : _unknowns) {
    cache[id] = type_id();
  }

  for (const auto &[id, tid] : _facts) {
    cache[id] = tid;
  }

  for (const auto &[a, b] : _links) {
    bool a_strong = cache.strong_check(a);
    bool b_strong = cache.strong_check(b);

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
        cache[a] = type_id::poison;
        cache[b] = type_id::poison;
      }
    }
  }

  for (const auto [id, tid] : _hints) {
    if (!cache.strong_check(id)) {
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

std::vector<sigil_node_id> TypeSync::linked(sigil_node_id root) {
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
