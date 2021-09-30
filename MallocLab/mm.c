/*
 * My implementation takes much of the textbook code and only slightly 
 * adapts the preexisting functions (except for the heavily modified find fit
 * function). Additionally, very simple functions to maintain the segregated list
 * are added (add, delete, set_seg_index, select_seg_list, set_seg_list). The last
 * three functions are akin to getters and setters of the seg list pointers.
 * 
 * Segregated free list: 10 buckets (10 global pointers)
 * Best-fit: Search from lower bucket to higher buckets, 
 *			 choose the best fit among the first 3 fits,
 *			 or return best fit so far when reaching a higher bucket.
 *
 * Note: MANY FUNCTIONS AND MACROS ARE DIRECTLY FROM THE TEXTBOOOK WITH SLIGHT MODIFICATIONS 
 *		 TO IMPLEMENT SEGREGATED FREE LIST AND FOR OPTIMIZATIONS
 * 			- mm_init
 * 			- mm_malloc
 * 			- mm_free
 * 			- mm_realloc
 * 			- mm_checkheap
 * 			- extend_heap
 * 			- place
 * 			- printblock
 *			- checkblock
 */

#include "memlib.h"
#include "mm.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Your info */
team_t team = {
    /* First and last name */
    "Brendan Rossmango",
    /* UID */
    "505370692",
    /* Custom message (16 chars) */
    "xqcL",
};

/* $begin mallocmacros */
/* Basic constants and macros */
#define WSIZE       4       /* Word and header/footer size (bytes) */ 
#define DSIZE       8       /* Doubleword size (bytes) */
#define CHUNKSIZE  (1<<10)  /* Initial heap size (bytes) */ 
#define LIST_COUNT	10     	/* Number of segregated free lists */

/* Payload pointers are aligned to 8 bytes */
#define ALIGNMENT 8
/* Nearest multiple of ALIGNMENT to return 8-byte aligned pointers */
#define ALIGN(p) (((size_t)(p) + (ALIGNMENT-1)) & ~0x7) // last 3 bits are 0

#define MIN_BLK_SIZE 24

#define MAX(x, y) ((x) > (y) ? (x) : (y))  
#define MIN(x, y) ((x) < (y) ? (x) : (y))  

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc)  ((size) | (alloc))

/* Read and write a word at address p */
#define GET(p)       (*(unsigned int *)(p))            
#define PUT(p, val)  (*(unsigned int *)(p) = (val))    

/* Read the size and allocated fields from address p */
#define GET_SIZE(p)  (GET(p) & ~0x7)                   
#define GET_ALLOC(p) (GET(p) & 0x1)                    

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp)   ((char *)(bp) - WSIZE)                      
#define FTRP(bp)   ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE) 

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp)  ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE))) 
#define PREV_BLKP(bp)  ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE))) 

/* Given block ptr bp, compute address of next and previous free blocks */
#define NEXT_FREEBLKP(bp)  (*(char **)(bp)) 
#define PREV_FREEBLKP(bp)  (*(char **)(bp + DSIZE)) 

/* Given block ptr bp and ptr, set next and previous blocks of a free block
   to ptr */
#define SET_NEXT_FREEBLK(bp, ptr) 	(NEXT_FREEBLKP(bp) = ptr)
#define SET_PREV_FREEBLK(bp, ptr)  	(PREV_FREEBLKP(bp) = ptr)
/* $end mallocmacros */

/* Global variables */
static char *heap_listp;  /* Pointer to first block */
/* The global "array" of pointers */  
static char *seg_list0;
static char *seg_list1;
static char *seg_list2;
static char *seg_list3;
static char *seg_list4;
static char *seg_list5;
static char *seg_list6;
static char *seg_list7;
static char *seg_list8;
static char *seg_list9;

/* function prototypes for internal helper routines */
static void *extend_heap(size_t words);
static void place(void *bp, size_t asize);
static void *find_fit(size_t asize);
static void *coalesce(void *bp);

/* function prototypes for internal helper routines
   for checking and debugging */
static void printblock(void *bp);
static void checkblock(void *bp);
static void checklist(void);

/* function prototypes for internal helper routines
   for segregated free list */
static void add(void *bp);
static void delete(void *bp);
static int seg_index(size_t asize);
static char* select_seg_list(size_t asize);
void set_seg_list(size_t asize, void* ptr);

/*
 * mm_init - Initialize the memory manager
 */
/* $begin mminit */
int mm_init(void) {
  	/* Create the initial empty heap */
  	if ((heap_listp = mem_sbrk(4*WSIZE)) == NULL)
		return -1; 

  	PUT(heap_listp, 0);                      /* Alignment padding */
  	PUT(heap_listp + WSIZE, PACK(DSIZE, 1)); /* Prologue header */ 
  	PUT(heap_listp + DSIZE, PACK(DSIZE, 1)); /* Prologue footer */ 
  	PUT(heap_listp + WSIZE + DSIZE, PACK(0, 1)); /* Epilogue header */
  	heap_listp += DSIZE;

  	/* Create the initial empty segregated list */
	seg_list0 =	NULL;
	seg_list1 =	NULL;
	seg_list2 =	NULL;
	seg_list3 =	NULL;
	seg_list4 =	NULL;
	seg_list5 =	NULL;
	seg_list6 =	NULL;
	seg_list7 =	NULL;
	seg_list8 =	NULL;
	seg_list9 =	NULL;

  	/* Extend the empty heap with a free block of CHUNKSIZE/4 bytes 
	 * instead of CHUNKSIZE bytes (for slightly better utilization)
	 */
  	if (extend_heap(CHUNKSIZE/(WSIZE*4)) == NULL)  
   		return -1;

  	return 0;
}
/* $end mminit */

/*
 * mm_malloc - Allocate a block with at least size bytes of payload, 
 * return block ptr bp
 */
/* $begin mmmalloc */
void *mm_malloc(size_t size) {
	size_t asize, extendsize; 	/* Amount to extend heap if no fit */
  	char *bp;

  	/* Ignore spurious requests */
  	if (size <= 0)
   		return NULL;
	
	/* Adjust block size to be the min block size or be aligned */
	if (size <= DSIZE)
		asize = MIN_BLK_SIZE;
	else
		asize = ALIGN(size + DSIZE);

  	/* Search for a fit in the free lists */
  	if ((bp = find_fit(asize)) != NULL)
   		place(bp, asize);                  

  	/* No fit found. Get more memory and place the block */
  	/* If the heap is small, extend by asize; else use MIN of heapsize and
	 * CHUNKSIZE (slightly better utilization than just CHUNKSIZE)
	 */
  	else{ 
		extendsize = MAX(asize, MIN(mem_heapsize(), CHUNKSIZE)); 
	  	if ((bp = extend_heap(extendsize/WSIZE)) == NULL)
	   		return NULL;
	  	place(bp, asize);
	}
 	return bp;
}
/* $end mmmalloc */

/*
 * mm_free: Free the block pointed to by payload and coalesce
 */
/* $begin mmfree */
void mm_free(void *payload) {
  	size_t size = GET_SIZE(HDRP(payload));

  	PUT(HDRP(payload), PACK(size, 0));
  	PUT(FTRP(payload), PACK(size, 0));
  	coalesce(payload);
}
/* $end mmfree */

/*
 * mm_realloc - naive implementation of mm_realloc
 * NO NEED TO CHANGE THIS CODE!
 */
void *mm_realloc(void *ptr, size_t size) {
  	void *newp;
  	size_t copySize;

  	if ((newp = mm_malloc(size)) == NULL) {
    	printf("ERROR: mm_malloc failed in mm_realloc\n");
    	exit(1);
  	}
  	copySize = GET_SIZE(HDRP(ptr));
  	if (size < copySize)
    	copySize = size;
  	memcpy(newp, ptr, copySize);
  	mm_free(ptr);
  	return newp;
}

/*
 * mm_checkheap: check heap consistency and free list consistency
 */
void mm_checkheap(int verbose) {
  	char *bp = heap_listp;
  	if (verbose)
		printf("Heap (%p):\n", heap_listp);

  	/* Check prologue */
  	if ((GET_SIZE(HDRP(heap_listp)) != DSIZE) || !GET_ALLOC(HDRP(heap_listp))){
		printf("Bad prologue header\n");
 	}
  	checkblock(heap_listp);

  	/* Check each block in the heap */
  	for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
		if (verbose) 
			printblock(bp);
		checkblock(bp);
  	}

  	/* Check epilogue */
  	if ((GET_SIZE(HDRP(bp)) != 0) || !(GET_ALLOC(HDRP(bp)))){
		printf("Bad epilogue header\n");
  	}

  	/* Check the segregated free list */
  	checklist();
}

/*
 * Print header and footer of block.
 */
static void printblock(void *bp) {
  	size_t hsize, halloc, fsize, falloc;

  	hsize = GET_SIZE(HDRP(bp));
  	halloc = GET_ALLOC(HDRP(bp));  
  	fsize = GET_SIZE(FTRP(bp));
  	falloc = GET_ALLOC(FTRP(bp));  

  	if (hsize == 0) {
   		printf("%p: EOL\n", bp);
   		return;
  	}

  	printf("%p: header: [%d:%c] footer: [%d:%c]\n", bp, 
	    (int)hsize, (halloc ? 'a' : 'f'), 
		(int)fsize, (falloc ? 'a' : 'f')); 
}

/*
 * Called in mm_checkheap
 * Checks if a single block is in heap, aligned, and header-footer
 */
static void checkblock(void *bp){
	/* Check if in the heap */       
    if (bp > mem_heap_hi() || bp < mem_heap_lo())
   		printf("ERROR: %p is not in mem heap \n", bp);     

  	/* Check alignment */
    if ((size_t)ALIGN(bp) != (size_t)bp)
  		printf("Error: %p is not doubleword aligned\n", bp);

  	/* Check header-footer */
  	if (GET(HDRP(bp)) != GET(FTRP(bp)))
		printf("Error: header does not match footer\n");
}

 /* 
  * Called in mm_checkheap
  * Checks the segregated free list (blocks must be free)
  */
static void checklist(void){ 
	char *bp;
	
	for (int i = 0; i < LIST_COUNT; i++){
		if (i == 0) bp = seg_list0;
		else if (i == 1) bp = seg_list1;
		else if (i == 2) bp = seg_list2;
		else if (i == 3) bp = seg_list3;
		else if (i == 4) bp = seg_list4;
		else if (i == 5) bp = seg_list5;
		else if (i == 6) bp = seg_list6;
		else if (i == 7) bp = seg_list7;
		else if (i == 8) bp = seg_list8;
		else if (i == 9) bp = seg_list9;		
		while (bp != NULL){
			checkblock(bp);
			if (GET_ALLOC(HDRP(bp))) {
			    printf("ERROR %p: allocated block in free list.\n", bp);	
				while (bp != NULL){
					printblock(bp);
					bp = NEXT_FREEBLKP(bp);
				}
			    exit(-1);
			}
			bp = NEXT_FREEBLKP(bp);
		}
	}
}

/* 
 * extend_heap - Extend heap with free block and return its block pointer
 */
/* $begin mmextendheap */
static void *extend_heap(size_t words){
 	char *bp;
  	size_t size;

  	/* Allocate an even number of words to maintain alignment */
  	size = (words % 2) ? (words+1) * WSIZE : words * WSIZE; 
    if ((bp = mem_sbrk(size)) == (void *)-1) return NULL;                                        

  	/* Initialize free block header/footer and the epilogue header */
  	PUT(HDRP(bp), PACK(size, 0));         /* Free block header */
  	PUT(FTRP(bp), PACK(size, 0));         /* Free block footer */
  	PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); /* New epilogue header */ 

  	/* Coalesce if the blocks are free */
  	return coalesce(bp);
}
/* $end mmextendheap */

/* 
 * place - Place block of asize bytes at start of free block bp and
 *         split if remainder would be at least minimum block size.
 */
/* $begin mmplace */
static void place(void *bp, size_t asize){
  	size_t csize = GET_SIZE(HDRP(bp)); 

	delete(bp);
  	if ((csize - asize) >= MIN_BLK_SIZE){ 
		PUT(HDRP(bp), PACK(asize, 1));
	  	PUT(FTRP(bp), PACK(asize, 1));
	  	bp = NEXT_BLKP(bp);
	  	PUT(HDRP(bp), PACK(csize - asize, 0));
	  	PUT(FTRP(bp), PACK(csize - asize, 0));
		add(bp);
  	} 
  	else{ 
		PUT(HDRP(bp), PACK(csize, 1));
		PUT(FTRP(bp), PACK(csize, 1));
  	} 
}
/* $end mmplace */

/* 
 * find_fit - Find a fit for a block with asize bytes.
 *			  Search from the lowest segregated list index and
 * 			  return the best one of first 3 fits in one bucket,
 *   		  or NULL if no fit found.
 * Finding the most optimized find_fit function
 *	 Return the best fit of first N fits in one bucket:
 * 		- First 1/2 fits: utilization too low even though it is the fastest
* 		- First 3 fits: best balance between utilization and throughput
 * 		- First 4/5 fits: almost best utilization, but slower than first 3 fits
 * 		- First 6 fits: best utilization, but rather slow
 * 		- First >6 fits: same utilization as 6, but even slower
 */

static void *find_fit(size_t asize){
 	char *bp;
  	char *best = NULL;
  	unsigned int size_difference = -1;
  	int count = 0;
	int index = seg_index(asize);
  	/* Start from the segregated list index based on asize */
  	for (int i = index; i < LIST_COUNT; i++){
  		/* If best != NULL, it means that the best fit search reached a 
		  higher bucket than the initial bucket based on asize */
    	if (best != NULL) return best;
		switch(i){
			case 0:
				bp = seg_list0;
				break;
			case 1:
				bp = seg_list1;
				break;
			case 2:
				bp = seg_list2;
				break;
			case 3:
				bp = seg_list3;
				break;
			case 4:
				bp = seg_list4;
				break;
			case 5:
				bp = seg_list5;
				break;
			case 6:
				bp = seg_list6;
				break;
			case 7:
				bp = seg_list7;
				break;
			case 8:
				bp = seg_list8;
				break;
			case 9:
				bp = seg_list9;
				break;
		}
    	/* Return best fit out of first 3 fits in same bucket 
		 * First time finding best fit: finds if the current block's size is at least 1 less than the inputted block size
		 * Second time finding best fit: finds if the current block's size is at least the previously found size difference
		 * less than the inputted block size
		 * Third time: same as second time
		 */
		while (bp != NULL){
   			if (asize <= GET_SIZE(HDRP(bp))){
   				if ((GET_SIZE(HDRP(bp)) - asize) < size_difference){
		    		best = bp;
		    		size_difference = GET_SIZE(HDRP(bp)) - asize;
		    	}
		    	count++;
		    	if (count > 3)
		    		return best;
	   		}
			bp = NEXT_FREEBLKP(bp);
		}
 	}
  	return best;
}

/*
 * coalesce - Boundary tag coalescing, return ptr to coalesced block.
 * The 4 cases of coalescing are achieved in a cleaner way.
 */
static void *coalesce(void *bp){
  	char *prev = PREV_BLKP(bp);
  	char *next = NEXT_BLKP(bp);
  	size_t prev_alloc = GET_ALLOC(FTRP(prev));
  	size_t next_alloc = GET_ALLOC(HDRP(next));
  	size_t size = GET_SIZE(HDRP(bp));

  	if (!prev_alloc){
    	bp = prev;
		size += GET_SIZE(HDRP(prev));
	  	/* Delete prev from segregated free list */
		delete(prev); 
		PUT(HDRP(bp), PACK(size, 0));
		PUT(FTRP(bp), PACK(size, 0));
  	}
  	if (!next_alloc){      
	  	size += GET_SIZE(HDRP(next));
		/* Delete next from segregated free list */
  		delete(next);
	  	PUT(HDRP(bp), PACK(size, 0));
	  	PUT(FTRP(bp), PACK(size, 0));
  	}
	/* Add bp to segregated free list */
  	add(bp);
	return bp;
}

/*
 * Add a free block last-in-first-out to segregated free list.
 */
static void add(void *bp){
	size_t size = GET_SIZE(HDRP(bp));
	char* list = select_seg_list(size);
	if (list == NULL){ 	
		SET_PREV_FREEBLK(bp, NULL);
		SET_NEXT_FREEBLK(bp, NULL);
	} 
	else{
		SET_PREV_FREEBLK(bp, NULL);   
		SET_NEXT_FREEBLK(bp, list);
		SET_PREV_FREEBLK(list, bp);
	}
	set_seg_list(size, bp);
}

/*
 * Delete a free block from segregated free list.
 */
static void delete(void *bp){
	char *prev = PREV_FREEBLKP(bp);
	char *next = NEXT_FREEBLKP(bp);
	size_t size = GET_SIZE(HDRP(bp));

	SET_PREV_FREEBLK(bp, NULL);
	SET_NEXT_FREEBLK(bp, NULL);

	if (prev == NULL){
		if (next == NULL)
			set_seg_list(size, NULL);
   		else{
			/* First free block */
			set_seg_list(size, next);
			SET_PREV_FREEBLK(next, NULL);
		}
	}
	else if (next == NULL){
		/* Last free block */
		SET_NEXT_FREEBLK(prev, NULL);
	}
	else{
		SET_PREV_FREEBLK(next, prev);
		SET_NEXT_FREEBLK(prev, next);
	}
}

/*
 * Return segregated list index based on asize.
 */
static int seg_index(size_t asize){
	if (asize <= 32) return 0;
	else if (asize <= 64) return 1;
	else if (asize <= 128) return 2;
	else if (asize <= 256) return 3;
	else if (asize <= 512) return 4;
	else if (asize <= 1024)	return 5;
	else if (asize <= 2048)	return 6;
	else if (asize <= 4096)	return 7;
	else if (asize <= 8192)	return 8;
	else return 9;
}

/*
 * Return segregated list based on asize.
 */
static char* select_seg_list(size_t asize){
	if (asize <= 32) return seg_list0;
	else if (asize <= 64) return seg_list1;
	else if (asize <= 128) return seg_list2;
	else if (asize <= 256) return seg_list3;
	else if (asize <= 512) return seg_list4;
	else if (asize <= 1024)	return seg_list5;
	else if (asize <= 2048)	return seg_list6;
	else if (asize <= 4096)	return seg_list7;
	else if (asize <= 8192)	return seg_list8;
	else return seg_list9;
}

/*
 * Set segregated list to ptr based on asize.
 */
void set_seg_list(size_t asize, void* ptr){
	if (asize <= 32) seg_list0 = ptr;
	else if (asize <= 64) seg_list1 = ptr;
	else if (asize <= 128) seg_list2 = ptr;
	else if (asize <= 256) seg_list3 = ptr;
	else if (asize <= 512) seg_list4 = ptr;
	else if (asize <= 1024)	seg_list5 = ptr;
	else if (asize <= 2048)	seg_list6 = ptr;
	else if (asize <= 4096)	seg_list7 = ptr;
	else if (asize <= 8192)	seg_list8 = ptr;
	else seg_list9 = ptr;
}