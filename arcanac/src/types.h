#pragma once

#include "arcana.h"
#include "pass/name.h"
#include "pass/types.h"
#include <sigil.h>

void report_types(const arcana::Tokens &, const arcana::Ast &,
                  const sigil::Overlay<arcana::pass::NamePass::Name> &scopes,
                  const arcana::pass::TypeDefPass &);
