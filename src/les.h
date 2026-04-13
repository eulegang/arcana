#ifndef _LES_H
#define _LES_H

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  const char *data;
  size_t len;
} les_slice;

typedef uint32_t les_token_type;

typedef struct {
  les_token_type type;
  uint16_t off;
  uint16_t len;
} les_token;

typedef ssize_t (*les_tokenizer)(size_t cur, les_slice content,
                                 les_token_type *type);

typedef struct {
  les_slice content;
  les_tokenizer tokenizer;
} les_tokens_options;

extern size_t les_pages;

typedef struct les_tokens les_tokens_t;
typedef struct les_token_table les_token_table_t;

les_tokens_t *les_tokens_init(les_tokens_options);
void les_tokens_deinit(les_tokens_t *);
size_t les_tokens_len(les_tokens_t *);
les_token *les_tokens_data(les_tokens_t *);

les_token_table_t *les_token_table_init();
void les_token_table_deinit(les_token_table_t *);

const char **les_token_table_data(les_token_table_t *);
size_t les_token_table_len(les_token_table_t *);
void les_token_table_push(les_token_table_t **, const char *);

#ifdef __cplusplus
}
#endif
#endif
