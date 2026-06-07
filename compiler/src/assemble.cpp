#include "assemble.h"
#include <fcntl.h>
#include <format>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

void assemble(std::string content, std::string_view file) {
  int pipes[2];
  pipe(pipes);

  int fd = open(file.data(), O_CREAT | O_EXCL | O_WRONLY, 0644);
  if (fd == -1) {
    std::string msg = std::format("failed to generate {}", file);
    perror(msg.c_str());
    exit(2);
  }

  pid_t pid = fork();
  if (pid == 0) {
    close(pipes[1]);
    dup2(pipes[0], 0);
    dup2(fd, 1);
    close(fd);

    execlp("llc", "llc", "--filetype=obj", (char *)NULL);
    perror("failed to assemble with llc");
    exit(1);
  } else if (pid < 0) {
    perror("failed to assemble with llc");
    exit(1);
  } else {
    close(pipes[0]);
    char *base = content.data();
    size_t w = 0;
    size_t len = content.length();

    while (w < len) {
      ssize_t l = write(pipes[1], base, len);
      if (l == -1) {
        perror("failed to write to assembler");
      }

      w += l;
    }
    close(pipes[1]);

    int status;
    waitpid(pid, &status, 0);

    std::cout << status << std::endl;
  }
}
