#include "util.h"
#include "arcana.h"

#include <string>

bool operator==(const arcana_slice slice, std::string_view view) {
  std::string_view slice_view(slice.data, slice.len);

  return slice_view == view;
}

std::ostream &operator<<(std::ostream &os, const arcana_slice slice) {
  std::string repr(slice.data, slice.len);

  return os << repr;
}
