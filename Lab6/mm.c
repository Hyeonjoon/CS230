/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/* Global variables for the segregated list */
static long **block1;
static long **block2;
static long **block3;
static long **block4;
static long **block5;
static int *block_buf;

/*
 * trav_list - Traverse the given seg-list and return the proper free block.
 *             If found, return pointer to the block and if not, return NULL.
 */
static long *trav_list(long **block, size_t size)
{
        long *ptr = *block;
	
	while((ptr >= (long *)mem_heap_lo()) && (ptr < (long *)mem_heap_hi())){ 
//        while ((ptr != NULL) && (ptr > (long *)(block_buf + 1))){
                // Found the proper free block.
                if ((*ptr == size) || (*ptr >= size + 24)){
                        return ptr;
                // This free block is smaller than required.
                // Check the next free block.
                }else{
			// Check if next is valid free block.
			long *next = *(long **)(ptr + 3);
			if ((next < (long *)mem_heap_lo()) || (next > (long *)mem_heap_hi())){
				next = NULL;
			}else if (((size_t)next % 8) != 0){
				next = NULL;
			}else{
				long *next_footer = (long *)((char *)next + *(next) - ALIGNMENT);
				if ((next_footer < (long *)mem_heap_lo()) || (next_footer > (long *)mem_heap_hi())){
					next = NULL;
				}else if ((size_t)next_footer % 8 != 0){
					next = NULL;
				}else if (*(next) != *(next_footer)){
					next = NULL;
				}
			}
			ptr = next;
                }
        }
        ptr = NULL;
        return ptr;
}

/*
 * trav_all - For a gliven seg-list block, call trav_list.
 *            If there's no proper block in that seg-list block,
 *            call trav_list with next seg-list until the end of
 *            the last seg-list block.
 *            (If next seg-list block is buffer, return NULL).
 */
static long *trav_all(long **block, size_t size)
{
	long **ptr_seglist;
	long **ptr_nextlist;
	long *ptr;
	// pointer to head of given seg-list class.
	ptr_seglist = block;
	// Not an end of the whole seg-list.
	while((int)(*ptr_seglist) != -1){
		// Cannot find proper free block in this class.
		if ((ptr = trav_list(ptr_seglist, size)) == NULL){
			// next seg-list class.
			ptr_nextlist = ptr_seglist + 2;
			ptr_seglist = ptr_nextlist;
		}else{
			return ptr;
		}
	// End of the whole seg-list.
	// Cannot find.
	}
	return NULL;
}

/*
 * find_block - Simply find the seg-list block
 *              with the given size.
 */
static long **find_block(size_t size)
{
	// Required size is less than 1B.
	// malloc should return NULL.
	if (size < 24){
		return NULL;
	// Required size is aligned to 8 and header is considered.
	// Required size is 24 ~ 509B.
	}else if (size < 510){
		return block1;
	// Required size is 510 ~ 1019B.
	}else if (size < 1020){
		return block2;
	// Required size is 1020 ~ 2039B.
	}else if (size < 2040){
		return block3;
	// Required size is 2040 ~ 4079B.
	}else if (size < 4080){
		return block4;
	// Required size is more than 4080B.
	}else{
		return block5;
	}
}

/* 
 * mm_init - Initialize the malloc package.
 *           Make seg-list.
 */
int mm_init(void)
{
	// start pointing the first byte of the heap.
	void *start = mem_sbrk(48);
	if (start == NULL){
		return -1;
	}
	// Head of blocks class with size 1 ~ 2 bytes.
	block1 = (long **)start;
	*(block1 - 1) = NULL;
	*(block1) = NULL;
	// Head of blocks class with size 3 ~ 4 bytes.
	block2 = (long **)(start + 8);
	*(block2 - 1) = NULL;
	*(block2) = NULL;
	// Head of blocks class with size 5 ~ 8 bytes.
	block3 = (long **)(start + 16);
	*(block3 - 1) = NULL;
	*(block3) = NULL;
	// Head of blocks class with size 9 ~ 16 bytes.
	block4 = (long **)(start + 24);
	*(block4 - 1) = NULL;
	*(block4) = NULL;
	// Head of blocks class with size 17 ~ bytes.
	block5 = (long **)(start + 32);
	*(block5 - 1) = NULL;
	*(block5) = NULL;
	// Buffer block for the trav_all to call the trav_list with next seglist class.
	block_buf = (int *)(start + 40);
	*(block_buf - 1) = -1;
	*(block_buf) = -1;
	return 0;
}

/* 
 * mm_malloc - Allocate a block by traverse seg-list.
 *             If there's no proper block in seg-list, incrementing the brk pointer.
 *             Always allocate a block whose size is a multiple of the alignment.
 *             If block allocated in seg-list is larger than needed size,
 *             split the block and free the last block.
 */
void *mm_malloc(size_t size)
{
	//mm_check();
	int newsize;
	long **block_target;
	long *block_allocated;
	newsize = ALIGN(size + SIZE_T_SIZE);
	// If nexsize is 16 bytes, expand it to 24 bytes.
        // When the 8 bytes block is freed, footer will overwite
        // the pointer to next and prev.
        // Thus allocated block should be larger than or equal to 24 bytes.
	if (newsize < 24){
		newsize += ALIGNMENT;
	}
	// If required size is 0, malloc returns NULL.
	if ((block_target = find_block(newsize)) == NULL){
		return NULL;
	}else{
		// Traverse the seg-list and get the pointer to the proper free block.
		// If there's no proper free block in that seg-list class,
		// trav_all calls trav_list with next seg-list class until the end of the seg-list.
		// If there's no proper free block in the whole seg-list, call sbrk.
		if ((block_allocated = trav_all(block_target, newsize)) == NULL){
			if ((block_allocated = (long *)mem_sbrk(newsize)) == NULL){
				return NULL;
			}
			// If block_allocated is made by sbrk, set the header.
			// And then return.
			*(block_allocated) = (newsize | 1);
			return (void *)(block_allocated) + ALIGNMENT;
		}
		// Blocks allocated by sbrk cannot reach here.
		// First, disconnect block_allocated with seg-list.
		// If malloc split the free block, pass the free block to the 'mm_free'.
		// 'mm_free' will append it to the seg-list.

		// Disconnect the allocated block with seg-list.
		// And connect prev and next elements in the seg-list each other.
		long *prev = (long *)(*(block_allocated + 2));
		long *next = (long *)(*(block_allocated + 3));
		// In this case, prev is pointer to a seg-list class.
		// seg-list classes have pointer to next block in different position.
		if (prev < (long *)block_buf){
			long **next_prev = (long **)(next + 2);
			long **prev_next = (long **)(prev);
			if ((next > (long *)mem_heap_lo()) && (next < (long *)mem_heap_hi())){
				*(next_prev) = prev;
			}
			*(prev_next) = next;
		// In this case, prev is not a pointer to a seg-list class.
		}else{
			long **next_prev = (long **)(next + 2);
			long **prev_next = (long **)(prev + 3);
			if ((next > (long *)mem_heap_lo()) && (next < (long *)mem_heap_hi())){
				*(next_prev) = prev;
			}
			*(prev_next) = next;
		}
		// Now prev and next of block_allocated are interconnected.
		// Disconnect block_allocated's one-side referencing.
		long **point_prev = (long **)(block_allocated + 2);
		long **point_next = (long **)(block_allocated + 3);
		*(point_prev) = NULL;
		*(point_next) = NULL;
		// Get oldsize and set header of the allocated block.
		int oldsize = (*(block_allocated) & -2);
		*(block_allocated) = (newsize | 1);
		// If this allocation splits free block,(thus this block is freed after allocation)
		// pass the splited free block to 'mm_free'.
		// 'mm_free' will append it to the seg-list.
		if (oldsize > newsize){
			// Set header and footer of the splited block.
			// And then pass it to the 'mm_free'.
			long *block_splited = (long *)((char *)block_allocated + newsize);
			*(block_splited) = oldsize - newsize;
			mm_free((void *)block_splited + ALIGNMENT);
		}
		// Return the starting address of payload.
		// Size and alloc bits are saved in 8 bytes.
		return (void *)(block_allocated) + ALIGNMENT;
	}
}

/*
 * mm_free - Freeing a block.
 *           Check if the prev and next of given block is also free blocks.
 *           If free, coalesce and append it to seg-list.
 */
void mm_free(void *ptr)
{
	long newsize;
	long *prev;
	long *curr;
	long *next;
	long *prev_footer;
	long *next_footer;
	long **target_list;	

	curr = (long *)(ptr - ALIGNMENT);
	next = (long *)((char *)curr + (*(curr) & -2));
	prev_footer = (long *)((char *)curr - ALIGNMENT);
	// Check if prev is valid block.
	// Check if prev block's footer is NULL or block_buf.
	if ((*(prev_footer) != 0) && (*(prev_footer) != -1)){
		prev = (long *)((char *)prev_footer - *(prev_footer) + ALIGNMENT);
		// Check if prev block is out of heap range.
		// Thus, check if content of prev block's footer is valid.
		if ((prev < (long *)mem_heap_lo()) || (prev > (long *)mem_heap_hi())){
			prev = NULL;
		// Check if prev block is aligned to 8.
		// Thus, check if content of prev block's footer is valid.
		}else if ((size_t)prev % 8 != 0){
			prev = NULL;
		// Check if prev block's header and footer have same contents.
		// Thus check if content of prev block's footer is valid.
		}else if (*(prev) != *(prev_footer)){
			prev = NULL;
		}
	}else{
		prev = NULL;
	}
	// Check if next is valid block.
	// Check if next block is out of heap range.
	// Thus, check if next block is unallocated heap region.
	if ((next < (long *)mem_heap_lo()) || (next > (long *)mem_heap_hi())){
		next = NULL;
	}else{
		next_footer = (long *)((char *)next + *(next) - ALIGNMENT);
		// Check if next block is aligned to 8.
		// Thus check if content of next block's header is valid.
		if ((size_t)(*next) % 8 != 0){
			next = NULL;
		// Check if next block's footer is out of heap range.
		// Thus check if content of next block's header is valid.
		}else if ((next_footer < (long *)mem_heap_lo()) || (next_footer > (long *)mem_heap_hi())){
			next = NULL;
		// Check if next block's header and footer have same contents.
		// Thus, check if content of next block's header is valid.
		}else if (*(next) != *(next_footer)){
			next = NULL;
		}
	}
	// size before coalescing.
	newsize = (*(curr) & -2);
	// If both prev and next are not free blocks,
	// no coalescing.
	// Just masking the alloc bit.
	if (prev == NULL || (((*(prev) & 1) != 0) || (*(prev) == 0))){
		if (next == NULL || (((*(next) & 1) != 0) || (*(next) == 0))){
			newsize = (*(curr) & -2);
		}
	}
	// If prev is free block, coalesce.
	// Check if prev has footer.
	// (Thus, if prev is free block and is not a unused allocated block.)
	// If curr is first allocated memory,
        // prev is block_buf of which all bits are 1.
        // Change curr to point to header of the coalesced block.
	// And computing new size.
	// Disconnect prev's connections in seg-list.
	if (prev != NULL && ((*(prev) & 1) == 0 && (*(prev) != 0))){
		long **point_prev;
		long *prev_prev = (long *)(*(prev + 2));
		long *prev_next = (long *)(*(prev + 3));
		// prev is next of seg-list.
		if (prev_prev < (long *)block_buf){
			// prev's next is not NULL.
			if ((prev_next > (long *)mem_heap_lo()) && (prev_next < (long *)mem_heap_hi())){
				if (*(prev_next) % 8 == 0){
					point_prev = (long **)(prev + 3);
					*(*(point_prev) + 2) = *(prev + 2);
				}
			}
			point_prev = (long **)(prev + 2);
			*(*(point_prev)) = *(prev + 3);
		// prev is not a next of seg-list.
		}else{
			// prev's next is not NULL.
			if ((prev_next > (long *)mem_heap_lo()) && (prev_next < (long *)mem_heap_hi())){
				if (*(prev_next) % 8 == 0){
					point_prev = (long **)(prev + 3);
					*(*(point_prev) + 2) = *(prev + 2);
				}
			}
			point_prev = (long **)(prev + 2);
			*(*(point_prev) + 3) = *(prev + 3);
		}
		// Now next and prev of prev are interconnected.
		// Disconnect prev's one-side referencing.
		long **point_prev_prev = (long **)(prev + 2);
		long **point_prev_next = (long **)(prev + 3);
		*(point_prev_prev) = NULL;
		*(point_prev_next) = NULL;
		// Change newsize and curr.
		// Initialize the prev and curr's header and footer(if exists).
		*(curr) = 0;
		*(prev_footer) = 0;
		newsize += *(prev);
		*(prev) = 0;
		curr = prev;
	}
	// If next is free block, coalesce.
	// Check if next has header.
	// (Thus, if next is free block and is not a unallocated heap area.)
	// No change of curr.
	// Just computing new size.
	// Disconnect next's connections in seg-list.
	if (next != NULL && (((*(next) & 1) == 0) && (*(next) != 0))){
		long **point_next;
		long *next_prev = (long *)(*(next + 2));
		long *next_next = (long *)(*(next + 3));
		// next is next of seg-list.
		if (next_prev < (long *)block_buf){
			// next's next is not NULL.
			if ((next_next > (long *)mem_heap_lo()) && (next_next < (long *)mem_heap_hi())){
				if (*(next_next) % 8 == 0){
					point_next = (long **)(next + 3);
					*(*(point_next) + 2) = *(next + 2);
				}
			}
			point_next = (long **)(next + 2);
			*(*(point_next)) = *(next + 3);
		// next is not a next of seg-list.
		}else{
			// next's next is not NULL.
			if ((next_next > (long *)mem_heap_lo()) && (next_next < (long *)mem_heap_hi())){
				if (*(next_next) % 8 == 0){
					point_next = (long **)(next + 3);
					*(*(point_next) + 2) = *(next + 2);
				}
			}
			point_next = (long **)(next + 2);
			*(*(point_next) + 3) = *(next + 3);
		}
		// Now nex and prev of next are interconnected.
		// Disconnect next's one-side referencing.
		long **point_next_prev = (long **)(next + 2);
		long **point_next_next = (long **)(next + 3);
		*(point_next_prev) = NULL;
		*(point_next_next) = NULL;
		// Change newsize.
		// Initialize the next and curr's header and footer(if exists).
		long *next_footer = (long *)((char *)next + *(next) - ALIGNMENT);
		//*(curr) = 0;
		*(next_footer) = 0;
		newsize += *(next);
		*(next) = 0;
	}
	// Now, change header and footer.
	// curr or newsize or both can be changed if there's at coalescing.
	long *header = curr;
	long *footer = (long *)(((char *)curr) + newsize - ALIGNMENT);
	*(header) = newsize;
	*(footer) = newsize;
	
	target_list = find_block(newsize);
	
	// Check if target_list is NULL.
	// If newsize is less than 24 bytes, find_block returns NULL.
	// That is for checking mm_malloc(0), not for free.
	// If find_block returns NULL, make target_list block1.
	if (target_list == NULL){
		target_list = block1;
	}
	
	long **curr_next = (long **)(curr + 3);
	long **curr_prev = (long **)(curr + 2);
	*(curr_next) = *(target_list);// curr's next.
	*(curr_prev) = (long *)target_list;// curr's prev.
	*(target_list) = curr;// seg-list's next
	if ((*(curr_next) > (long *)mem_heap_lo()) && (*(curr_next) < (long *)mem_heap_hi())){
		*(long **)(*(curr_next) + 2) = curr;// next seg-list element's prev is curr.
	}
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
	void *old_ptr = ptr;
	void *new_ptr;
	
	// If ptr is NULL, it is equivalent to mm_malloc(size).
	if (old_ptr == NULL){
		new_ptr = mm_malloc(size);
		return new_ptr;
	// If size is 0, it is equivalent to mm_free(ptr).
	}else if (size == 0){
		mm_free(old_ptr);
		return NULL;
	// ptr is not a NULL, and size is not equal to 0.
	}else{
		long *header = (long *)(old_ptr - ALIGNMENT);
		long old_size = (*(header) & -2);
		long new_size = ALIGN(size + SIZE_T_SIZE);
		if (new_size < 24){
			new_size += ALIGNMENT;
		}
		// realloc with same size.
		// Just return given pointer.
		if (old_size == new_size){
			return old_ptr;
		// realloc with smaller size.
		}else if (old_size > new_size){
			if (old_size < new_size + 24){
				return old_ptr;
			}
			new_ptr = old_ptr;
			long *block_splited = (long *)(old_ptr + new_size - ALIGNMENT);
			*block_splited = old_size - new_size;
			mm_free((void *)block_splited + ALIGNMENT);
			long *block_allocated = (long *)(new_ptr - ALIGNMENT);
			*block_allocated = (new_size | 1);
			return new_ptr;
		// realloc with larger size.
		}else{
			new_ptr = mm_malloc(size);
			size_t copy_size = old_size - ALIGNMENT;
			memcpy(new_ptr, old_ptr, copy_size);
			mm_free(old_ptr);
			return new_ptr;
		}
	}
}












