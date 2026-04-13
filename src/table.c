#include <assert.h>
#include <stdlib.h>

#include "les.h"

struct les_token_table {
  size_t len;
  size_t cap;
};

#define TABLE_DEFAULT_LEN 1

les_token_table_t *les_token_table_init() {
  size_t len = sizeof(char *) * TABLE_DEFAULT_LEN + sizeof(les_token_table_t);
  les_token_table_t *table = malloc(len);

  if (!table)
    return NULL;

  table->cap = TABLE_DEFAULT_LEN;
  table->len = 0;
  return table;
}

void les_token_table_deinit(les_token_table_t *table) { free(table); }

size_t les_token_table_len(les_token_table_t *table) {
  assert(table);
  return table->len;
}

const char **les_token_table_data(les_token_table_t *table) {
  assert(table);
  return (const char **)(table + 1);
}

void les_token_table_push(les_token_table_t **table, const char *sym) {
  assert(table);
  assert(*table);

  les_token_table_t *t = *table;

  if (t->len >= t->cap) {
    t->cap *= 2;
    t = realloc(t, sizeof(les_token_table_t) + sizeof(const char *) * t->cap);
    *table = t;
  }

  *(les_token_table_data(t) + t->len) = sym;
  t->len += 1;
}
