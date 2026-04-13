#ifndef _MONKEY_H
#define _MONKEY_H

#include "les.h"
#include <cstddef>
#include <cstdint>

enum monkey_token_type : uint32_t {
  monkey_token_type_let,
  monkey_token_type_assign,
  monkey_token_type_semi,
  monkey_token_type_ident,
  monkey_token_type_number,

};

ssize_t monkey_tokenizer(size_t cur, les_slice content, les_token_type *type);

#endif
