#include "../generate.h"

namespace gen {
Definition::Emitter Emitter::define(const Definition &def) {
  out << "define " << def.ret << " @" << def.name << "(";

  auto x = def.args.begin();

  if (x != def.args.end()) {
    auto [type, name] = *x;
    out << type << " %" << name;
  }

  while (x != def.args.end()) {
    out << ", ";

    auto [type, name] = *x;
    out << type << " %" << name;

    x++;
  }

  out << ") {" << std::endl;

  return Definition::Emitter{out};
}

} // namespace gen
