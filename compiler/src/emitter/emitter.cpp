#include "../generate.h"

namespace lir {
Definition::Emitter Emitter::define(const Definition &def) {
  out << "define " << def.ret << " @" << def.name << "(";

  auto it = def.args.begin();

  if (it != def.args.end()) {
    out << *it++;
  }

  while (it != def.args.end()) {
    out << ", " << *it++;
  }

  out << ") {" << std::endl;

  return Definition::Emitter{out};
}

void Emitter::declare(const Declaration &declare) {
  out << "declare " << declare.ret << " @" << declare.name << "(";

  auto it = declare.args.begin();

  if (it != declare.args.end()) {
    out << *it++;
  }

  while (it != declare.args.end()) {
    out << ", " << *it++;
  }

  out << ")" << std::endl;
}

void Emitter::global(std::string name, Typed<Lit> lit) {
  out << "@" << name << " = " << lit << std::endl;
}

void Emitter::type(std::string name, std::string type) {
  out << "%" << name << " = type " << type << std::endl;
}

} // namespace lir
