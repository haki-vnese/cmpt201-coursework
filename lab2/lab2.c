#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
  char *buffer = NULL;
  size_t buffer_size = 0;

  printf("Enter a program to run: \n");

  while (1) {
    printf("> ");

    ssize_t chars_read = getline(&buffer, &buffer_size, stdin);

    // Case: Could not getline()
    if (chars_read == -1) {
      break;
    }

    // Case: user only hits Enter with no input
    if (chars_read == 1 && buffer[0] == '\n') {
      break;
    }

    char *saveptr = NULL;
    char *program = strtok_r(buffer, "\n", &saveptr);

    if (program == NULL) {
      break;
    }

    pid_t pid = fork();

    if (pid < 0) {
      perror("fork");
      exit(EXIT_FAILURE);
    } else if (pid == 0) {
      execl(program, program, NULL);

      printf("Execl failed!\n");
      exit(1);
    } else {
      waitpid(pid, NULL, 0);
    }
  }
  free(buffer);

  return 0;
}
