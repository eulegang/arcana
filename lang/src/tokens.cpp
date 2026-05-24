#include "arcana.h"
#include <sigil.h>

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
    if ((inc = sigil_util_keyword(window, "=="))) {
      *token_type = token(eq);
      return inc;
    }

    *token_type = token(assign);
    return 1;

  case '!':
    if ((inc = sigil_util_keyword(window, "!="))) {
      *token_type = token(ne);
      return inc;
    }

    *token_type = token(bang);
    return 1;

  case '*':
    if ((inc = sigil_util_keyword(window, "*="))) {
      *token_type = token(mult_assign);
      return inc;
    }

    *token_type = token(mult);
    return 1;

  case '-':
    if ((inc = sigil_util_keyword(window, "->"))) {
      *token_type = token(arrow);
      return inc;
    }

    if ((inc = sigil_util_keyword(window, "-="))) {
      *token_type = token(minus_assign);
      return inc;
    }

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

    if ((inc = sigil_util_keyword(window, "/="))) {
      *token_type = token(div_assign);
      return inc;
    }

    *token_type = token(div);
    return 1;

  case '"': {
    int state = 0;
    *token_type = token(str);
    return sigil_util_take_stateful(window, &state, arcana::end_str);
  } break;

  case '%':
    if ((inc = sigil_util_keyword(window, "%="))) {
      *token_type = token(mod_assign);
      return inc;
    }

    *token_type = token(mod);
    return 1;

  case ',':
    *token_type = token(comma);
    return 1;

  case 'a':
    if ((inc = sigil_util_keyword(window, "alias"))) {
      *token_type = token(alias);
      return inc;
    }
    break;

  case 'b':
    if ((inc = sigil_util_keyword(window, "bitset"))) {
      *token_type = token(bitset);
      return inc;
    }
    break;

  case 'e':
    if ((inc = sigil_util_keyword(window, "enum"))) {
      *token_type = token(enumeration);
      return inc;
    }
    break;

  case 'f':
    if ((inc = sigil_util_keyword(window, "fn"))) {
      *token_type = token(fn);
      return inc;
    }
    if ((inc = sigil_util_keyword(window, "false"))) {
      *token_type = token(bool_f);
      return inc;
    }
    break;

  case 'l':
    if ((inc = sigil_util_keyword(window, "let"))) {
      *token_type = token(let);
      return inc;
    }
    break;

  case 't':
    if ((inc = sigil_util_keyword(window, "true"))) {
      *token_type = token(bool_t);
      return inc;
    }

    break;

  case 'n':
    if ((inc = sigil_util_keyword(window, "namespace"))) {
      *token_type = token(ns);
      return inc;
    }
    break;

  case 'o':
    if ((inc = sigil_util_keyword(window, "opaque"))) {
      *token_type = token(opaque);
      return inc;
    }
    break;

  case 's':
    if ((inc = sigil_util_keyword(window, "struct"))) {
      *token_type = token(strukt);
      return inc;
    }
    break;

  case 'v':
    if ((inc = sigil_util_keyword(window, "var"))) {
      *token_type = token(var);
      return inc;
    }
    break;

  case ':':
    if ((inc = sigil_util_keyword(window, "::"))) {
      *token_type = token(dcolon);
      return inc;
    }

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
    if ((inc = sigil_util_keyword(window, "&&="))) {
      *token_type = token(bool_and_assign);
      return inc;
    }

    if ((inc = sigil_util_keyword(window, "&&"))) {
      *token_type = token(bool_and);
      return inc;
    }

    break;

  case '|':
    if ((inc = sigil_util_keyword(window, "||="))) {
      *token_type = token(bool_or_assign);
      return inc;
    }

    if ((inc = sigil_util_keyword(window, "||"))) {
      *token_type = token(bool_or);
      return inc;
    }

    break;

  case '+':
    if ((inc = sigil_util_keyword(window, "+="))) {
      *token_type = token(plus_assign);
      return inc;
    }

    *token_type = token(plus);
    return 1;
  }

  if (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ch == '_') {
    *token_type = token(ident);
    return sigil_util_take_while(window, arcana::is_ident);
  }

  return 0;
}
} // namespace arcana
