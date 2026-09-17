#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_history(char *history[], int count) {
  for (int i = 0; i < count; i++) {
    printf("%s\n", history[i]);
  }
}

void add_history(char *history[], int *count, char *line) {
  if (history == NULL || count == NULL || line == NULL) {
    return;
  }

  if (*count < 0 || *count > 5) {
    return;
  }

  if (*count < 5) {
    history[*count] = line;
    (*count)++;
  } else {
    free(history[0]);

    for (int i = 0; i < 4; i++) {
      history[i] = history[i + 1];
    }

    history[4] = line;
  }
}
void free_history(char *history[], int *count) {
  if (history == NULL || *count == 0) {
    printf("Can not free an empty history\n");
    return;
  }

  for (int i = 0; i < *count; i++) {
    free(history[i]);
    history[i] = NULL;
  }
  *count = 0;
}

char *get_input(void) {
  while (1) {
    char *line = NULL;
    size_t size = 0;

    printf("Enter input: ");
    ssize_t chars_read = getline(&line, &size, stdin);
    // If getline() fails
    if (chars_read == -1) {
      if (ferror(stdin)) {
        perror("getline");
      }

      free(line);
      return NULL;
    }

    if (strcmp(line, "\n") == 0) {
      printf("Please enter an input!\n");
      free(line);
      continue;
    }

    return line;
  }
}
int main(void) {

  char *history[5] = {NULL};
  int count = 0;

  while (1) {
    char *line = get_input();

    if (line == NULL) {
      break;
    }

    if (strcmp(line, "clear\n") == 0) {
      free_history(history, &count);

      if (count != 0) {
        printf("Failed to clear history\n");
      } else {
        printf("History cleared!\n");
      }

      continue;
    }
    add_history(history, &count, line);

    if (strcmp(line, "print\n") == 0) {
      print_history(history, count);
    }
  }
  free_history(history, &count);
  return 0;
}
