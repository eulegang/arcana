module sigil {
  alias token_type = u32;
  alias token_id = u16;

  record linemeta {
    line: u16;
    column: u16;
  }

  record token {
    type: token_type;
    off: u16;
    len: u16;
  }


  alias tokenizer = func(cur: u64, content: []u8, type: *token_type) -> i64;

  record token_options {
    content: []u8;
    tokenizer: tokenizer;
  }
}
