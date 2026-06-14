foreign "write" func cwrite(u32, *u8, u64) -> i64;

func write(fd: u32, slice: []u8) -> void!i64{
  n := cwrite(fd, slice.data, slice.ptr);
  if n == -1 {
    throw;
  }

  return n;
}

func main(args: []*u8) -> u32 {
  msg : []u8 : "hello world\n";
  stdout : u32 : 1;
  write(stdout, msg);

  return 0;
}
