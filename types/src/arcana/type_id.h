#pragma once

#include <cstdint>
#include <stdexcept>

namespace arcana::types {
struct type_id {

#define MASK 0xF0000000
  enum class cat : uint16_t {
    meta = 0,
    bs = 1,
    en = 2,
    st = 3,
    prim = 4,
    derive = 5,
    fn = 6,
    alias = 7,
  };

  type_id() { payload = 0; }
  type_id(cat category, uint16_t id) {
    if (MASK & id) {
      throw std::overflow_error("type id overflow");
    }

    payload = ((uint16_t)category << 28) | id;
  }

  cat category() const { return (cat)((MASK & payload) >> 28); }
  uint16_t id() const { return (~MASK & payload); }
  operator bool() const { return payload != 0; }

  bool operator==(const type_id &other) const {
    return payload == other.payload;
  }

  type_id operator++() {
    uint32_t next = payload + 1;
    if ((MASK & next) != (MASK & payload))
      throw std::overflow_error("type id overflow");

    payload = next;
    return *this;
  }

  type_id operator++(int) {
    type_id res{};
    res.payload = payload;

    uint32_t next = payload + 1;
    if ((MASK & next) != (MASK & payload))
      throw std::overflow_error("type id overflow");

    payload = next;
    return res;
  }

  bool operator<(const type_id &other) const { return payload < other.payload; }

  static type_id null;
  static type_id poison;
  static type_id unit;
  static type_id boolean;

private:
  uint32_t payload;

#undef MASK
};

std::ostream &operator<<(std::ostream &, const type_id &);
} // namespace arcana::types
