/*
 * Memory Allocator: explicit free list + FIFO
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

#define WSIZE 4
#define DSIZE 8           /*Double word size*/
#define CHUNKSIZE (1<<12) /*the page size in bytes is 4K*/

#define MAX(x, y)    ((x)>(y)?(x):(y))

#define PACK(size, alloc)    ((size) | (alloc))

#define GET(p)  (*(unsigned int *)(p))
#define PUT(p, val)  (*(unsigned int *)(p) = (val))

#define GET_SIZE(p)  (GET(p) & ~0x7)
#define GET_ALLOC(p)    (GET(p) & 0x1)

#define HDRP(bp)    ((char *)(bp)-WSIZE)
#define FTRP(bp)    ((char *)(bp)+GET_SIZE(HDRP(bp))-DSIZE)

#define PREV_LINKNODE_RP(bp) ((char*)(bp))
#define NEXT_LINKNODE_RP(bp) ((char*)(bp)+WSIZE)

#define NEXT_BLKP(bp)   ((char *)(bp)+GET_SIZE(((char *)(bp)-WSIZE)))
#define PREV_BLKP(bp)   ((char *)(bp)-GET_SIZE(((char *)(bp)-DSIZE)))

int mm_check(char *function);

static void *extend_heap(size_t words);

static void *coalesce(void *bp);

static void *find_fit(size_t size);

static void place(void *bp, size_t asize);

void insert_to_list(char *p);

void remove_from_list(char *p);

static char *heap_listp = NULL;
static char *root = NULL;

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void) {
    if ((heap_listp = mem_sbrk(6 * WSIZE)) == (void *) -1) return -1;

    PUT(heap_listp, 0);
    PUT(heap_listp + (1 * WSIZE), 0);
    PUT(heap_listp + (2 * WSIZE), 0);
    PUT(heap_listp + (3 * WSIZE), PACK(DSIZE, 1));
    PUT(heap_listp + (4 * WSIZE), PACK(DSIZE, 1));
    PUT(heap_listp + (5 * WSIZE), PACK(0, 1));

    root = heap_listp + WSIZE;
    heap_listp += (4 * WSIZE);

    if ((extend_heap(CHUNKSIZE / DSIZE)) == NULL) return -1;

    return 0;
}


static void *extend_heap(size_t words) {
    char *bp;
    size_t size;

    size = (words % 2) ? (words + 1) * DSIZE : words * DSIZE;

    if ((long) (bp = mem_sbrk(size)) == -1)
        return NULL;

    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    PUT(NEXT_LINKNODE_RP(bp), 0);
    PUT(PREV_LINKNODE_RP(bp), 0);

    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));

    return coalesce(bp);
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size) {
    size_t asize;
    size_t extendsize;
    char *bp;
    if (size == 0) return NULL;

    if (size <= DSIZE)
        asize = 2 * DSIZE;
    else
        asize = (DSIZE) * ((size + (DSIZE) + (DSIZE - 1)) / (DSIZE));

    if ((bp = find_fit(asize)) != NULL) {
        place(bp, asize);
        return bp;
    }

    extendsize = MAX(asize, CHUNKSIZE);
    if ((bp = extend_heap(extendsize / DSIZE)) == NULL) return NULL;

    place(bp, asize);
    return bp;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *bp) {
    if (bp == 0) return;

    size_t size = GET_SIZE(HDRP(bp));

    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    PUT(NEXT_LINKNODE_RP(bp), 0);
    PUT(PREV_LINKNODE_RP(bp), 0);
    coalesce(bp);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size) {
    size_t oldsize;
    void *newptr;

    /* If size == 0 then this is just free, and we return NULL. */
    if (size == 0) {
        mm_free(ptr);
        return 0;
    }

    /* If oldptr is NULL, then this is just malloc. */
    if (ptr == NULL)
        return mm_malloc(size);

    oldsize = GET_SIZE(HDRP(ptr));
    newptr = mm_malloc(size);

    /* If realloc() fails the original block is left untouched  */
    if (!newptr) return 0;

    oldsize = GET_SIZE(HDRP(ptr));
    if (size < oldsize) oldsize = size;
    memcpy(newptr, ptr, oldsize);

    mm_free(ptr);
    return newptr;
}


/*coalesce the empty block*/
static void *coalesce(void *bp) {
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if (prev_alloc && next_alloc) {
        return bp;
    } else if (prev_alloc && !next_alloc) {
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        remove_from_list(NEXT_BLKP(bp));    /* remove from linked list */
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
    } else if (!prev_alloc && next_alloc) {
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        remove_from_list(PREV_BLKP(bp));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    } else {
        size += GET_SIZE(FTRP(NEXT_BLKP(bp))) + GET_SIZE(HDRP(PREV_BLKP(bp)));
        remove_from_list(PREV_BLKP(bp));
        remove_from_list(NEXT_BLKP(bp));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    insert_to_list(bp);
    return bp;
}


// insert p at the beginning of linked list
void insert_to_list(char *p) {
    char *nextp = GET(root);
    if (nextp != NULL)
        PUT(PREV_LINKNODE_RP(nextp), p);

    PUT(NEXT_LINKNODE_RP(p), nextp);
    PUT(root, p);
}


// remove p from linked list
void remove_from_list(char *p) {
    char *prevp = GET(PREV_LINKNODE_RP(p));
    char *nextp = GET(NEXT_LINKNODE_RP(p));

    if (prevp == NULL) {
        if (nextp != NULL)
            PUT(PREV_LINKNODE_RP(nextp), 0);
        PUT(root, nextp);
    } else {
        if (nextp != NULL)
            PUT(PREV_LINKNODE_RP(nextp), prevp);
        PUT(NEXT_LINKNODE_RP(prevp), nextp);
    }

    PUT(NEXT_LINKNODE_RP(p), 0);
    PUT(PREV_LINKNODE_RP(p), 0);
}

static void *find_fit(size_t size) {
    char p = GET(root);
    while (p != NULL) {
        if (GET_SIZE(HDRP(p)) >= size) return p;
        p = GET(NEXT_LINKNODE_RP(p));
    }
    return NULL;
}

static void place(void *bp, size_t asize) {
    size_t csize = GET_SIZE(HDRP(bp));
    remove_from_list(bp);

    if ((csize - asize) >= (2 * DSIZE)) {
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        bp = NEXT_BLKP(bp);

        PUT(HDRP(bp), PACK(csize - asize, 0));
        PUT(FTRP(bp), PACK(csize - asize, 0));
        PUT(NEXT_LINKNODE_RP(bp), 0);
        PUT(PREV_LINKNODE_RP(bp), 0);
        coalesce(bp);
    } else {
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
    }
}