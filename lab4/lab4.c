#define _DEFAULT_SOURCE
#define BUF_SIZE 100

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct header {
  uint64_t size;
  struct header *next;
};

void print_out(char *format, void *data, size_t data_size) {
  char buf[BUF_SIZE];
  ssize_t len = snprintf(buf, BUF_SIZE, format,
                         data_size == sizeof(uint64_t) ? *(uint64_t *)data : *(void **)data);

  if (len < 0) {
    write(STDERR_FILENO, "snprintf", 16);
    exit(1);
  }

  write(STDOUT_FILENO, buf, len);
}

void traverse_blocks(struct header *current) {
  while (current != NULL) {
    size_t data_size = 128 - sizeof(struct header);
    char *data = (char *)current + sizeof(struct header);

    print_out("block address: %p\n", &current, sizeof(current));
    print_out("block size: %lu\n", &current->size, sizeof(current->size));
    print_out("block next: %p\n", &current->next, sizeof(current->next));

    print_out("current block data address: %p\n", &data, sizeof(data));

    for (size_t i = 0; i < data_size; i++) {
      uint64_t value = (unsigned char)data[i];
      print_out("%lu\t", &value, sizeof(value));
    }
    current = current->next;
  }
}
int main() {
  char buf[32];
  write(STDOUT_FILENO, "How many blocks do you need? ", 29);
  ssize_t bytes_read = read(STDIN_FILENO, buf, sizeof(buf) - 1);

  if (bytes_read <= 0) {
    exit(1);
  }
  buf[bytes_read] = '\0';

  int num_blocks = atoi(buf);

  if (num_blocks <= 0) {
    write(STDERR_FILENO, "Invalid number\n", 15);
    exit(1);
  }

  void *start = sbrk(num_blocks * 128);

  if (start == (void *)-1) {
    perror("sbrk");
    exit(1);
  }

  for (int i = 0; i < num_blocks; i++) {
    struct header *current = (struct header *)((char *)start + i * 128);

    current->size = 128;

    if (i == 0) {
      current->next = NULL;
    } else {
      current->next = (struct header *)((char *)start + (i - 1) * 128);
    }

    char *data = (char *)current + sizeof(struct header);
    memset(data, i, 128 - sizeof(struct header));
  }

  struct header *last = (struct header *)((char *)start + (num_blocks - 1) * 128);
  traverse_blocks(last);

  return 0;
}
