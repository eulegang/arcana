msg :: "hello world\n";
stdout : i32 = 1;

func write(fd: u32, slice: []u8) -> void!i64{
  n := cwrite(fd, slice.data, slice.len);
  if n == -1 {
    throw;
  }

  return n;
}

foreign "write" func cwrite(u32, *u8, u64) -> i64;

func main(args: []*u8) -> u32 {
  write(stdout, msg);

  return 0;
}
