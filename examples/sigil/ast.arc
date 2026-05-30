module sigil {
  record node {
    child: u16;
    next: u16;
    offset: u16;
    type: u16;
  }

  system ast {
    len: u64;
    cap: u64;
    nodes: u16;
    data: u16;

    alias visitor = fn(node: node, data: *void, level: u64, content: []u8, ctx: *void);
  }

  record state {
    tokens: *tokens;
    ast: *ast;

    token_cursor: u16;
    node_cursor: u16;
    data_cursor: u16;

    subroot: u16;
    status: u16;
  }

  record parser {
    len: u64;
    terminal: fn(type: token_type) -> bool;
    init: subparser;

    alias subparser = fn(state: state) -> state;
    alias binparser = fn(state: state, id: u16) -> state;

    record slot {
      prefix: subparser;
      postfix: subparser;
      infix: binparser;
      perc: u64;
    }

    record error {
      token: u16;
      status: u16;
    }
  }
}
