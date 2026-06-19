#include "../generate.h"
#include "gmock/gmock.h"

using Emitter = lir::Definition::Emitter;
using Reg = lir::Reg;
using Lit = lir::Lit;

std::ostream &operator<<(std::ostream &out, const lir::Reg &reg) {
  return out << reg.name;
}
std::ostream &operator<<(std::ostream &out, const lir::Global &g) {
  return out << g.name;
}

std::ostream &operator<<(std::ostream &out, const lir::Lit &lit) {
  return out << lit.name;
}

Emitter::~Emitter() { out << "}" << std::endl; }
Reg Emitter::stack(std::string type) {
  Reg cur = cur_inst++;
  out << "  " << cur << " = alloca " << type << std::endl;
  return cur;
}

Reg Emitter::gep(std::string type, Reg ptr, uint32_t field) {
  Reg cur = cur_inst++;
  out << "  " << cur << " = getelementptr " << type << ", ptr " << ptr
      << ", i32 0, i32 " << field << std::endl;

  return cur;
}

void Emitter::store(Reg dst, Typed<Reg> src) {
  out << "  store " << src.type << " " << src.value << ", ptr " << dst
      << std::endl;
}

Reg Emitter::load(Typed<Reg> src) {
  Reg cur = cur_inst++;
  out << "  " << cur << " = load " << src.type << ", ptr " << src.value
      << std::endl;
  return cur;
}

Reg Emitter::call(Typed<Global> func, std::vector<Typed<Reg>> args) {
  Reg cur = cur_inst++;
  out << "  " << cur << " = call " << func.type << " " << func.value << "(";

  auto it = args.begin();

  if (it != args.end()) {
    auto [type, arg] = *it;
    out << type << " " << arg;
    it++;
  }

  while (it != args.end()) {
    out << ", ";
    auto [type, arg] = *it;
    out << type << " " << arg;
    it++;
  }

  out << ")" << std::endl;

  return cur;
}

template <typename T> void Emitter::ret(Typed<T> func) {
  out << "  ret " << func << std::endl;
}

template void Emitter::ret<lir::Lit>(Typed<lir::Lit> func);
template void Emitter::ret<Reg>(Typed<Reg> func);

// void Emitter::ret(Typed<Lit> func) { out << "  ret " << func << std::endl; }
