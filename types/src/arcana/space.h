#pragma once

#include <span>
#include <stdexcept>
#include <vector>

namespace arcana::util {
template <typename T> class Space {
  std::vector<T> _backing;
  size_t _mask;

public:
  Space() : _backing{}, _mask{0} {}

  void push(T elem) {
    if (_mask != _backing.size())
      throw std::logic_error("adding to partial space");
    _backing.push_back(elem);
    _mask++;
  }

  void restore() { _mask = _backing.size(); }

  bool empty() { return _mask == 0; }

  T &pop() {
    auto ref = _backing[_mask--];
    __builtin_sub_overflow(_mask, 1, &_mask);
    return ref;
  }

  auto begin() { return _backing.begin(); }
  auto end() { return _backing.begin() + _mask; }

  auto cbegin() const { return _backing.cbegin(); }
  auto cend() const { return _backing.cbegin() + _mask; }

  void clear() {
    _backing.clear();
    _mask = 0;
  }

  operator std::span<T>() const {
    const std::span<T> sp{_backing};
    return sp.subspan(_mask);
  }
};
} // namespace arcana::util
