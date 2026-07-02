#include "parsers.h"
#include "gmock/gmock.h"
#include <cstdint>
#include <iterator>
#include <sigil.h>

namespace arcana {
sigil_state parse_func_sig_ret(sigil_state state);

sigil_state parse_func(sigil_state state) {
  check_token(func);
  next_token();

  auto [id, root] = alloc_node(fn);

  run_subparser(root, parse_ident, child);
  sigil_node *ident = sigil_state_node(state, root->child);

  auto [params_id, params] = alloc_node(fn_params);
  ident->next = params_id;

  check_token(lparen);
  next_token();

  sigil_node *cur = params;
  while (true) {
    loop_terminal_token(rparen);

    check_token(ident);

    auto [param_id, param] = alloc_node(fn_param);
    if (cur == params) {
      cur->child = param_id;
    } else {
      cur->next = param_id;
    }

    cur = param;

    run_subparser(param, parse_ident, child);

    check_token(colon);
    next_token();

    ident = sigil_state_node(state, param->child);

    run_subparser(ident, parse_type, next);

    if (token_is(comma)) {
      next_token();
    } else {
      check_token(rparen);
    }
  }

  run_subparser(params, parse_func_sig_ret, next);

  sigil_node *next =
      state.subroot ? sigil_state_node(state, state.subroot) : params;

  run_subparser(next, parse_block, next);

  state.subroot = id;
  return state;
}

sigil_state parse_func_type(sigil_state state) {
  check_token(func);
  next_token();

  auto [id, root] = alloc_node(fn);

  auto [params_id, params] = alloc_node(fn_params);
  root->child = params_id;
  check_token(lparen);
  next_token();

  sigil_node *cur = params;
  while (true) {
    loop_terminal_token(rparen);

    check_token(ident);

    auto [param_id, param] = alloc_node(fn_param);
    if (cur == params) {
      cur->child = param_id;
    } else {
      cur->next = param_id;
    }

    cur = param;

    if (token_is(ident)) {
      run_subparser(param, parse_ident, child);

      check_token(colon);
      next_token();
      sigil_node *ident = sigil_state_node(state, param->child);

      run_subparser(ident, parse_type, next);
    } else {
      run_subparser(param, parse_type, next);
    }

    if (token_is(comma)) {
      next_token();
    } else {
      check_token(rparen);
    }
  }

  run_subparser(params, parse_func_sig_ret, next);

  state.subroot = id;
  return state;
}

sigil_state parse_func_sig_ret(sigil_state state) {
  if (!token_is(arrow)) {
    state.subroot = 0;
    return state;
  }
  next_token();

  auto [id, root] = alloc_node(fn_ret);

  run_subparser(root, parse_type, child);

  if (!token_is(bang)) {
    state.subroot = id;
    return state;
  }
  next_token();

  sigil_node *ty = sigil_state_node(state, root->child);
  run_subparser(ty, parse_type, next);

  state.subroot = id;
  return state;
}

sigil_state parse_foreign(sigil_state state) {
  check_token(foreign);
  next_token();

  sigil_token_id c_ident = 0;
  if (token_is(str)) {
    c_ident = state.token_cursor;
    next_token();
  }

  check_token(func);
  next_token();

  auto [id, root] = alloc_node(foreign);

  auto [cname_id, cname_node] = alloc_node(infer_type);

  root->child = cname_id;
  if (c_ident) {
    cname_node->type = node_code(str);
    sigil_state_span(state, cname_id, {c_ident, c_ident});
  }

  run_subparser(cname_node, parse_ident, next);
  sigil_node *ident = sigil_state_node(state, cname_node->next);

  auto [params_id, params] = alloc_node(fn_params);
  ident->next = params_id;

  check_token(lparen);
  next_token();

  sigil_node *cur = params;
  while (true) {
    loop_terminal_token(rparen);

    auto [param_id, param] = alloc_node(fn_param);
    if (cur == params) {
      cur->child = param_id;
    } else {
      cur->next = param_id;
    }

    cur = param;

    ident = sigil_state_node(state, param->child);

    run_subparser(param, parse_type, child);

    if (token_is(comma)) {
      next_token();
    } else {
      check_token(rparen);
    }
  }

  run_subparser(params, parse_func_sig_ret, next);

  check_token(semi);
  next_token();

  state.subroot = id;
  return state;
}

sigil_state parse_func_call(sigil_state state, sigil_node_id node) {
  check_token(lparen);
  next_token();

  auto [id, root] = alloc_node(fn_call);

  root->child = node;

  auto [params_id, params] = alloc_node(fn_params);
  sigil_state_node(state, node)->next = params_id;

  sigil_node *cur = params;
  while (true) {
    loop_terminal_token(rparen);

    auto [param_id, param] = alloc_node(fn_param);
    if (cur == params) {
      cur->child = param_id;
    } else {
      cur->next = param_id;
    }

    cur = param;

    run_subparser(param, parse_expr, child);

    if (token_is(comma)) {
      next_token();
    } else {
      check_token(rparen);
    }
  }

  state.subroot = id;
  return state;
}

} // namespace arcana
//
