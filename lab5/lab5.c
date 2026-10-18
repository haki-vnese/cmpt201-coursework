#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct header {
  uint64_t size;
  struct header *next;
  int id;
};

void coalesce(struct header **free_list, struct header *block);

void initialize_block(struct header *block, uint64_t size, struct header *next, int id) {
  block->size = size;
  block->next = next;
  block->id = id;
}

int find_first_fit(struct header *free_list_ptr, uint64_t size) {
  // TODO: Implement first fit
  struct header *current = free_list_ptr;

  while (current != NULL) {
    if (current->size >= size) {
      return current->id;
    }

    current = current->next;
  }
  return -1;
}

int find_best_fit(struct header *free_list_ptr, uint64_t size) {
  int best_fit_id = -1;
  // TODO: Implement best fit

  uint64_t best_size = 0;

  struct header *current = free_list_ptr;

  while (current != NULL) {
    if (current->size >= size && (best_fit_id == -1 || current->size < best_size)) {
      best_size = current->size;
      best_fit_id = current->id;
    }

    current = current->next;
  }
  return best_fit_id;
}

int find_worst_fit(struct header *free_list_ptr, uint64_t size) {
  int worst_fit_id = -1;
  // TODO: Implement worst fit

  uint64_t worst_size = 0;

  struct header *current = free_list_ptr;

  while (current != NULL) {
    if (current->size >= size && (worst_fit_id == -1 || current->size > worst_size)) {
      worst_size = current->size;
      worst_fit_id = current->id;
    }

    current = current->next;
  }
  return worst_fit_id;
}

int main(void) {

  struct header *free_block1 = (struct header *)malloc(sizeof(struct header));
  struct header *free_block2 = (struct header *)malloc(sizeof(struct header));
  struct header *free_block3 = (struct header *)malloc(sizeof(struct header));
  struct header *free_block4 = (struct header *)malloc(sizeof(struct header));
  struct header *free_block5 = (struct header *)malloc(sizeof(struct header));

  initialize_block(free_block1, 6, free_block2, 1);
  initialize_block(free_block2, 12, free_block3, 2);
  initialize_block(free_block3, 24, free_block4, 3);
  initialize_block(free_block4, 8, free_block5, 4);
  initialize_block(free_block5, 4, NULL, 5);

  struct header *free_list_ptr = free_block1;

  int first_fit_id = find_first_fit(free_list_ptr, 7);
  int best_fit_id = find_best_fit(free_list_ptr, 7);
  int worst_fit_id = find_worst_fit(free_list_ptr, 7);

  // TODO: Print out the IDs

  printf("The ID for First-Fit algorithm is: %d\n", first_fit_id);
  printf("The ID for Best-Fit algorithm is: %d\n", best_fit_id);
  printf("The ID for Worst-Fit algorithm is: %d\n", worst_fit_id);

  free(free_block1);
  free(free_block2);
  free(free_block3);
  free(free_block4);
  free(free_block5);

  // PART 2: Coalescing tests
  // ==========================================

  const size_t a = 32, b = 48, c = 64;
  const size_t h = sizeof(struct header);

  // Allocate contiguous memory for three blocks.
  char *region = malloc(3 * h + a + b + c);

  if (region == NULL) {
    perror("malloc");
    return 1;
  }

  struct header *block1 = (struct header *)region;
  struct header *block2 = (struct header *)(region + h + a);
  struct header *block3 = (struct header *)(region + 2 * h + a + b);

  // TEST 1: Merge with both neighbors
  printf("\nTEST 1: Merge both sides\n");

  initialize_block(block1, a, block3, 1);
  initialize_block(block2, b, NULL, 2);
  initialize_block(block3, c, NULL, 3);

  struct header *free_list = block1;

  coalesce(&free_list, block2);

  printf("Expected: ID 1, size %zu\n", a + b + c + 2 * h);
  printf("Actual:   ID %d, size %llu\n", free_list->id, (unsigned long long)free_list->size);

  // TEST 2: Merge with previous block only
  printf("\nTEST 2: Merge previous only\n");

  initialize_block(block1, a, NULL, 1);
  initialize_block(block2, b, NULL, 2);
  initialize_block(block3, c, NULL, 3);

  free_list = block1;

  coalesce(&free_list, block2);

  printf("Expected: ID 1, size %zu\n", a + b + h);
  printf("Actual:   ID %d, size %llu\n", free_list->id, (unsigned long long)free_list->size);

  // TEST 3: Merge with next block only
  printf("\nTEST 3: Merge next only\n");

  initialize_block(block1, a, NULL, 1);
  initialize_block(block2, b, NULL, 2);
  initialize_block(block3, c, NULL, 3);

  free_list = block3;

  coalesce(&free_list, block2);

  printf("Expected: ID 2, size %zu\n", b + c + h);
  printf("Actual:   ID %d, size %llu\n", free_list->id, (unsigned long long)free_list->size);

  // TEST 4: No adjacent free blocks
  printf("\nTEST 4: No adjacent free blocks\n");

  initialize_block(block1, a, NULL, 1);
  initialize_block(block2, b, NULL, 2);
  initialize_block(block3, c, NULL, 3);

  free_list = block1;

  coalesce(&free_list, block3);

  printf("Expected: ID 1 -> ID 3 -> NULL\n");
  printf("Actual:   ");

  struct header *curr = free_list;

  while (curr != NULL) {
    printf("ID %d -> ", curr->id);
    curr = curr->next;
  }

  printf("NULL\n");

  // TEST 5: Empty free list
  printf("\nTEST 5: Empty free list\n");

  initialize_block(block1, a, NULL, 1);

  free_list = NULL;

  coalesce(&free_list, block1);

  printf("Expected: ID 1, size %zu\n", a);
  printf("Actual:   ID %d, size %llu\n", free_list->id, (unsigned long long)free_list->size);

  free(region);

  return 0;
}

/*
Coalescing Algorithm:

Assume the existing free list is already coalesced.
Each block size represents payload bytes.

1. Receive a newly freed block.

2. Search the free list to find:
   - A free block immediately before the new block
     in physical memory.
   - A free block immediately after the new block
     in physical memory.

3. If both blocks exist:
   - Merge the previous block, new block, and next block.
   - Update the previous block's size to include
     the two other payloads and their headers.
   - Remove the next block from the free list.
   - Do not insert the new block separately.

4. Else if only the previous block exists:
   - Increase the previous block's size to include
     the new block's header and payload.
   - Do not insert the new block separately.

5. Else if only the next block exists:
   - Increase the new block's size to include
     the next block's header and payload.
   - Replace the next block's entry in the free list
     with the new merged block.

6. Otherwise:
   - Insert the newly freed block into the free list.

7. Ensure all next pointers remain valid and no
   merged block appears twice in the free list.
*/

void coalesce(struct header **free_list, struct header *block) {
  struct header *prev = NULL;
  struct header *curr = *free_list; // curr = address of block 1

  // Find the position in memory-address order
  while (curr != NULL && (uintptr_t)curr < (uintptr_t)block) {
    prev = curr;
    curr = curr->next;
  }

  // Insert the newly freed block into the free list
  block->next = curr;

  if (prev != NULL) { // if there is a free block before the new block
    prev->next = block;
  } else { // if the new block is the head of free list
    *free_list = block;
  }

  // Check if the previous block is physically adjacent
  if (prev != NULL && (char *)prev + sizeof(struct header) + prev->size == (char *)block) {
    prev->size += sizeof(struct header) + block->size;
    prev->next = block->next;

    block = prev;
  }

  curr = block->next;
  // Check if the next block is physically adjacent
  if (curr != NULL && (char *)block + sizeof(struct header) + block->size == (char *)curr) {
    block->size += sizeof(struct header) + curr->size;
    block->next = curr->next;
  }
}
