module sigil {
  alias token_type = u32;

  record token {
    type: token_type;
    off: u16;
    len: u16;
  }

  alias tokenizer = fn(cur: u64, content: []u8, ty: *token_type) -> i64;

  record position {
    line: u16;
    column: u16;
  }

  system tokens {
    len: u64;
    content: []u8;
    cap: u64;

    record options {
      content: []u8;
      tokenizer: tokenizer;
    }

    record error {
      code: error_code;
      position: u32;
    }

    enum error_code : u32 {
      map;
      overflow;
      invalid;
    }
  }

  system table {
    len: u64;
    cap: u64;
  }

}
