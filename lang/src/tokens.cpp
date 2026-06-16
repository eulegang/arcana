#include "arcana.h"
#include <sigil.h>

#define check_keyword(Key, Token)                                              \
  if ((inc = sigil_util_keyword(window, Key))) {                               \
    *token_type = token(Token);                                                \
    return inc;                                                                \
  }

namespace arcana {
bool is_space(char ch) {
  return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
}

bool is_ident(char ch) {
  return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ch == '_' ||
         ('0' <= ch && ch <= '9');
}

bool is_integer(char ch) { return ('0' <= ch && ch <= '9'); }
bool not_newline(char ch) { return ch != '\n'; }

bool end_block_comment(void *st, char ch) {
  uint32_t *state = (uint32_t *)st;

  if (*state == 2) {
    return false;
  }

  switch (ch) {
  case '/': {
    if (*state == 1) {
      *state = 2;
    }
  } break;

  case '*':
    *state = 1;
    break;

  default:
    *state = 0;
  }

  return true;
}

bool end_str(void *st, char ch) {
  uint32_t *state = (uint32_t *)st;

  switch (*state) {
  case 0: {
    if (ch == '"') {
      *state = 1;
    }
  } break;

  case 1: {
    if (ch == '\\') {
      *state = 2;
    } else if (ch == '"') {
      *state = 3;
    }
  } break;
  case 2:
    *state = 1;
    break;

  case 3:
    return false;
  }

  return true;
}

#define token(T) arcana::Token::T
ssize_t tokenizer(size_t cur, sigil_slice content, arcana::Token *token_type) {
  sigil_slice window = sigil_slice_advance(content, cur);

  char ch = content.data[cur];
  ssize_t inc = 0;

  switch (ch) {
  case ' ':
  case '\t':
  case '\n':
  case '\r':
    return -sigil_util_take_while(window, arcana::is_space);

  case ';':
    *token_type = token(semi);
    return 1;

  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    *token_type = token(integer);

    return sigil_util_take_while(window, arcana::is_integer);
  case '=':
    check_keyword("==", eq);
    *token_type = token(assign);
    return 1;

  case '!':
    check_keyword("!=", ne);
    *token_type = token(bang);
    return 1;

  case '*':
    check_keyword("*=", mult_assign);
    *token_type = token(mult);
    return 1;

  case '-':
    check_keyword("->", arrow);
    check_keyword("-=", minus_assign);
    *token_type = token(minus);
    return 1;

  case '/':
    if (window.len > 1 && window.data[1] == '/') {
      return -sigil_util_take_while(window, arcana::not_newline);
    }

    uint32_t state;
    if (window.len > 1 && window.data[1] == '*') {
      return -sigil_util_take_stateful(window, &state,
                                       arcana::end_block_comment);
    }

    check_keyword("/=", div_assign);
    *token_type = token(div);
    return 1;

  case '"': {
    int state = 0;
    *token_type = token(str);
    return sigil_util_take_stateful(window, &state, arcana::end_str);
  } break;

  case '%':
    check_keyword("%=", mod_assign);
    *token_type = token(mod);
    return 1;

  case ',':
    *token_type = token(comma);
    return 1;

  case 'a':
    check_keyword("alias", alias);
    break;

  case 'b':
    check_keyword("bitset", bitset);
    break;

  case 'e':
    check_keyword("enum", enumeration);
    check_keyword("else", cond_else);
    break;

  case 'f':
    check_keyword("fn", fn);
    check_keyword("func", func);
    check_keyword("false", bool_f);
    check_keyword("foreign", foreign);
    break;

  case 'i':
    check_keyword("if", cond_if);
    break;

  case 'l':
    check_keyword("let", let);
    break;

  case 'm':
    check_keyword("module", module);
    check_keyword("mut", mut);
    break;

  case 'n':
    check_keyword("namespace", ns);
    break;

  case 'o':
    check_keyword("opaque", opaque);
    break;

  case 'p':
    check_keyword("priv", priv);
    break;

  case 'r':
    check_keyword("return", ret);
    check_keyword("record", record);
    break;

  case 's':
    check_keyword("struct", strukt);
    break;

  case 't':
    check_keyword("true", bool_t);
    check_keyword("throw", ret_err);
    break;

  case 'v':
    check_keyword("var", var);
    break;

  case ':':
    // check_keyword("::", dcolon);
    *token_type = token(colon);
    return 1;

  case '{':
    *token_type = token(lbrace);
    return 1;

  case '}':
    *token_type = token(rbrace);
    return 1;

  case '(':
    *token_type = token(lparen);
    return 1;

  case ')':
    *token_type = token(rparen);
    return 1;

  case '[':
    *token_type = token(lbracket);
    return 1;

  case ']':
    *token_type = token(rbracket);
    return 1;

  case '&':
    check_keyword("&&=", bool_and_assign);
    check_keyword("&&", bool_and);
    break;

  case '|':
    check_keyword("||=", bool_or_assign);
    check_keyword("||", bool_or);
    break;

  case '+':
    check_keyword("+=", plus_assign);
    *token_type = token(plus);
    return 1;

  case '.':
    *token_type = token(dot);
    return 1;
  }

  if (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ch == '_') {
    *token_type = token(ident);
    return sigil_util_take_while(window, arcana::is_ident);
  }

  return 0;
}
} // namespace arcana
