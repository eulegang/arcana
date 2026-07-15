
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

  void assert_same(Key dst, Key src) {
    auto a_ty = std::get<type_id>(mapping[src]);
    auto b_ty = std::get<type_id>(mapping[dst]);

    if (a_ty != b_ty) {
      mapping[src] = type_id::poison;
      mapping[dst] = type_id::poison;
    }
  }

  void broadcast_link(Key id, const TypeSync &sync) {
    const auto tid = std::get<type_id>(mapping[id]);
    for (const auto linked : sync.linked(id)) {
      mapping[linked] = tid;
    }
  }

  void unify(Key a, Key b, const TypeSync &sync) {
    bool a_strong = strong_check(a);
    bool b_strong = strong_check(b);

    if (!a_strong && !b_strong) {
      mapping[a] = b;
    } else if (a_strong && !b_strong) {
      broadcast_link(a, sync);
    } else if (b_strong && !a_strong) {
      broadcast_link(b, sync);
    } else {
      assert_same(a, b);
    }
  }
};

void TypeSync::push(sigil_node_id id) { _unknowns.push(id); }

void TypeSync::link(sigil_node_id dst, sigil_node_id src) {
  _links.push({dst, src});
}

void TypeSync::set(sigil_node_id dst, type_id tid) { _facts.push({dst, tid}); }

void TypeSync::hint(sigil_node_id dst, type_id tid) { _hints.push({dst, tid}); }

void TypeSync::compress() {
  InferCache cache;

  for (const auto id : _unknowns) {
    cache[id] = type_id();
  }

  for (const auto &[id, tid] : _facts) {
    cache[id] = tid;
  }

  for (const auto [a, b] : _links) {
    cache.unify(a, b, *this);
  }

  for (const auto [id, tid] : _hints) {
    if (!cache.strong_check(id)) {
      cache[id] = tid;
      for (const auto linked : linked(id)) {
        cache[linked] = tid;
      }
    }
  }

  // TODO: rework. just need an answer right now
  std::vector<size_t> pending;
  for (size_t i = 0; const auto &[dst, src, member] : _members) {
    auto sstrong = cache.strong_check(src);
    auto dstrong = cache.strong_check(dst);
    if (!sstrong && !dstrong) {
      cache[dst] = type_id::poison;
    } else if (!dstrong) {
      type_id tid = std::get<type_id>(cache[src]);
      cache[dst] = base.member(tid, member);
    } else {
      pending.push_back(i);
    }
  }

  size_t cached = pending.size();
  while (!pending.empty()) {

    if (cached == pending.size()) {
      // no work was done
    }

    cached = pending.size();
  }

  _facts.clear();
  for (const auto &[key, value] : cache) {
    if (auto v = std::get_if<type_id>(&value)) {
      _facts.push({key, *v});
    } else {
      auto id = std::get<sigil_node_id>(value);
      if (auto tid = std::get_if<type_id>(&cache[id])) {
        _facts.push({key, *tid});
      }
    }
  }
}

std::vector<sigil_node_id> TypeSync::linked(sigil_node_id root) const {
  std::vector<sigil_node_id> results;

  std::stack<sigil_node_id> fringe;
  fringe.push(root);

  while (!fringe.empty()) {
    auto cur = fringe.top();
    fringe.pop();

    for (auto it = _links.cbegin(); it != _links.cend(); ++it) {
      const auto [a, b] = *it;
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
