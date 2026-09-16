#define _POSIX_C_SOURCE_200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {

  char *buffer = NULL;
  size_t buffer_size = 0;

  while (1) {
    printf("Please enter some text: ");

    ssize_t chars_read = getline(&buffer, &buffer_size, stdin);

    if (buffer[0] == '\n')
      break;
    if (chars_read == -1) {
      if (feof(stdin)) { // if end of file, break the loop
        break;
      }
      perror("getline");
      free(buffer);
      return 1;
    }

    if (chars_read > 0 && buffer[chars_read - 1] == '\n')
      buffer[chars_read - 1] = '\0';

    printf("Tokens:\n");

    char *saveptr = NULL;
    char *token = strtok_r(buffer, " ", &saveptr);

    while (token != NULL) {
      printf("  %s\n", token);

      token = strtok_r(NULL, " ", &saveptr);
    }
  }
  free(buffer);

  return 0;
}
