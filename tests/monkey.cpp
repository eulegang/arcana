
#include "monkey.h"
#include "les.h"

les_slice pull_word(les_slice content, uint16_t offset) {
  les_slice s = {.data = content.data + (size_t)offset, .len = 0};

  while (offset + s.len < content.len) {
    char c = s.data[s.len];

    bool skip = false;

    skip |= 'a' <= c && c <= 'z';
    skip |= 'A' <= c && c <= 'Z';
    skip |= '0' <= c && c <= '9';

    if (!skip)
      break;

    s.len += 1;
  }

  return s;
}

les_slice pull_number(les_slice content, uint16_t offset) {
  les_slice s = {.data = content.data + (size_t)offset, .len = 0};

  while (offset + s.len < content.len) {
    char c = s.data[s.len];

    bool skip = false;

    skip |= '0' <= c && c <= '9';

    if (!skip)
      break;

    s.len += 1;
  }

  return s;
}

les_token_table_t *monkey_table() {
  static les_token_table_t *table = NULL;

  if (!table) {
    table = les_token_table_init();
    les_token_table_push(&table, "let");
  }

  return table;
}

ssize_t monkey_tokenizer(size_t cur, les_slice content, les_token_type *type) {
  const char ch = content.data[cur];
  switch (ch) {
  case ' ':
    return -1;
  case '=':
    *type = monkey_token_type_assign;
    return 1;

  case ';':
    *type = monkey_token_type_semi;
    return 1;

  case 'l':
    if (content.data[cur + 1] == 'e' && content.data[cur + 2] == 't') {
      *type = monkey_token_type_let;
      return 3;
    }
    break;
  }

  if (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ch == '_') {
    les_slice c = pull_word(content, cur);
    *type = monkey_token_type_ident;
    return c.len;
  }

  if ('0' <= ch && ch <= '9') {
    les_slice c = pull_number(content, cur);
    *type = monkey_token_type_number;
    return c.len;
  }

  return 0;
}
