#pragma once

#include <ostream>
#include <string>
#include <vector>

namespace lir {

struct Reg {
  std::string name;

  Reg(uint16_t reg) : name{std::format("%{}", reg)} {}
  Reg(std::string reg) : name{std::format("%{}", reg)} {}
  Reg(std::string_view reg) : name{std::format("%{}", reg)} {}
  Reg(const char *reg) : name{std::format("%{}", reg)} {}
};

struct Global {
  std::string name;

  Global(std::string reg) : name{std::format("@{}", reg)} {}
  Global(std::string_view reg) : name{std::format("@{}", reg)} {}
  Global(const char *reg) : name{std::format("@{}", reg)} {}
};

struct Lit {
  std::string name;

  Lit(uint16_t reg) : name{std::format("{}", reg)} {}
  Lit(std::string reg) : name{std::format("{}", reg)} {}
  Lit(std::string_view reg) : name{std::format("{}", reg)} {}
};

template <typename T> struct Typed {
  std::string type;
  T value;
};

struct Declaration {
  std::string ret;
  std::string name;
  std::vector<std::string> args;
};

struct Definition {
  std::string ret;
  std::string name;
  std::vector<Typed<Reg>> args;

  class Emitter {
    std::ostream &out;
    uint16_t cur_inst;

  public:
    Emitter(std::ostream &out) : out{out}, cur_inst{1} {}
    ~Emitter();

    Reg stack(std::string type);
    Reg gep(std::string type, Reg ptr, uint32_t field);
    void store(Reg dst, Typed<Reg> src);
    Reg load(Typed<Reg> reg);
    Reg call(Typed<Global> func, std::vector<Typed<Reg>> args);

    template <typename T> void ret(Typed<T> ret);
  };
};

struct Emitter {
  std::ostream &out;

  Emitter(std::ostream &out) : out{out} {}

  Definition::Emitter define(const Definition &def);
  void declare(const Declaration &declare);

  void global(std::string name, Typed<Lit> lit);
  void type(std::string name, std::string type);
};
} // namespace lir

std::ostream &operator<<(std::ostream &, const lir::Reg &);
std::ostream &operator<<(std::ostream &, const lir::Global &);
std::ostream &operator<<(std::ostream &, const lir::Lit &);

template <typename T>
std::ostream &operator<<(std::ostream &out, const lir::Typed<T> &ty) {
  return out << ty.type << " " << ty.value;
}
