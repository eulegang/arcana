#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include "les.h"

struct les_tokens {
  size_t len;
  les_slice content;
  size_t cap;
};

size_t les_pages = 16;

bool les_process(les_tokens_t *, les_tokens_options opts);

les_tokens_t *les_tokens_init(les_tokens_options opts) {
  assert(opts.content.data != NULL);
  assert(opts.content.len != 0);

  size_t len = les_pages * getpagesize();
  les_tokens_t *res =
      mmap(0, len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);

  if (res == MAP_FAILED) {
    return NULL;
  }

  res->len = 0;
  res->cap = len;
  res->content = opts.content;

  if (!les_process(res, opts)) {
    les_tokens_deinit(res);
    return NULL;
  }

  return res;
}

void les_tokens_deinit(les_tokens_t *tokens) { munmap(tokens, tokens->len); }

bool les_process(les_tokens_t *tokens, les_tokens_options opts) {
  uint16_t cur = 0;
  les_token_type type;

  les_token *base = les_tokens_data(tokens);
  do {
    ssize_t inc = opts.tokenizer(cur, opts.content, &type);
    if (inc == 0) {
      return false;
    } else if (inc < 0) {
      cur += -inc;
    } else {
      les_token t = {
          .type = type,
          .off = cur,
          .len = inc,
      };

      base[tokens->len++] = t;

      cur += inc;
    }

  } while (cur < opts.content.len);

  return true;
}

size_t les_tokens_len(les_tokens_t *table) { return table->len; }
les_token *les_tokens_data(les_tokens_t *table) {
  return (les_token *)((void *)table + sizeof(les_tokens_t));
}
