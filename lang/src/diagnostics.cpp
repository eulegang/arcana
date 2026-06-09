#include "arcana.h"

using namespace arcana;

Diagnostics::operator bool() const { return !diagnostics.empty(); }

bool Diagnostics::has_errors() const {
  for (const auto &diag : diagnostics) {
    if (diag.flags & 0x1) {
      return true;
    }
  }

  return false;
}

bool Diagnostics::has_warnings() const {
  for (const auto &diag : diagnostics) {
    if (!(diag.flags & 0x1)) {
      return true;
    }
  }

  return false;
}

void Diagnostics::add_error(std::string message, sigil_span span) {
  diagnostics.push_back(Diag{
      message,
      span,
      0x1,
  });
}

void Diagnostics::add_warning(std::string message, sigil_span span) {
  diagnostics.push_back(Diag{
      message,
      span,
      0x0,
  });
}
