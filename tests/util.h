#pragma once

#include <string_view>

#include "arcana.h"

bool operator==(const arcana_slice, std::string_view);
std::ostream &operator<<(std::ostream &os, const arcana_slice);
