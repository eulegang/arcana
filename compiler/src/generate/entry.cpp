#include "../generate.h"
#include <cstring>

void gen::EntryComponent::generate() {
  if (has_main()) {
    gen_main();
  }
}

bool gen::EntryComponent::has_main() {
  symbol s = 0xFFFF;
  for (symbol cur : unit.symbols) {
    if (strcmp(unit.symbols.resolve(cur), "main") == 0) {
      s = cur;
      break;
    }
  }

  if (s == 0xFFFF) {
    return false;
  }

  for (const auto &fn : unit.entries.bodies) {
    uint16_t i = unit.ast[fn.id].child;

    auto name = unit.overlays.names.resolve(i);

    if (name->_symbol == s && name->_parent == 0xFFFF) {
      return true;
    }
  }

  return false;
}

void gen::EntryComponent::gen_main() {
  auto sub = emitter.define({"i32",
                             "main",
                             {
                                 {"i64", "argc"},
                                 {"ptr", "argv"},
                             }});
  auto slice = "{ptr, i64}";
  auto args = sub.stack(slice);

  auto data = sub.gep(slice, args, 0);
  sub.store(data, {"ptr", "argv"});

  auto len = sub.gep(slice, args, 0);
  sub.store(len, {"i64", "argc"});

  args = sub.load({slice, args});

  auto res = sub.call({"i32", (lir::Global) "arcana.main"}, {{slice, args}});

  sub.ret({"i32", res});
}
