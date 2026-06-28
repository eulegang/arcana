#include "slate.h"
#include "gmock/gmock.h"
#include <sigil.h>
#include <stdexcept>

using namespace arcana::types;

void TypeSlate::clear() { slots.clear(); }
void TypeSlate::push(sigil_node_id id) {
  slots.push_back({
      id,
      std::monostate(),
  });
}

void TypeSlate::link(sigil_node_id dst, sigil_node_id src) {
  uint16_t dst_id = 0xFFFF;
  uint16_t src_id = 0xFFFF;

  for (uint16_t i = 0; i < slots.size(); i++) {
    Slot slot = slots[i];

    if (slot.node_id == dst) {
      dst_id = i;
    }

    if (slot.node_id == src) {
      src_id = i;
    }
  }

  if (src_id == dst_id)
    throw std::logic_error("cycle created");

  if (src_id == 0xFFFF || dst_id == 0xFFFF) {
    throw std::logic_error("invalid nodes linked");
  }

  slots[dst_id].value = src_id;
}

void TypeSlate::set(sigil_node_id dst, type_id tid) {
  uint16_t dst_id = 0xFFFF;

  for (uint16_t i = 0; i < slots.size(); i++) {
    Slot slot = slots[i];

    if (slot.node_id == dst) {
      dst_id = i;
    }
  }

  if (dst_id == 0xFFFF) {
    throw std::logic_error("invalid nodes set");
  }

  slots[dst_id].value = tid;
}
