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

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "LooxMaxxing",
    /* First member's full name */
    "Bruno Fernandes Iorio",
    /* First member's email address */
    "bruno.fernandes-iorio@polytechnique.edu",
    /* Second member's full name (leave blank if none) */
    "Marcel Chwiałkowski",
    /* Second member's email address (leave blank if none) */
    "marcel.chwialkowski@polytechnique.edu"
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8
//why can we not do 16? would be way better but ok

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

//macros from the textbook
#define PACK(size, alloc) ((size) | (alloc)) //packs a size and alloc into a word

#define GET(p) (*(unsigned int *)(p)) //gets the value in p
#define PUT(p, val) (*(unsigned int *)(p) = (val)) //puts a value into pointer p

#define GET_SIZE(p) (GET(p) & ~0x7) //gets size at p
#define GET_ALLOC(p) (GET(p) & 0x1) //gets alloc at p

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/* 
 * mm_init - initialize the malloc package.
 */
size_t lastbit = sizeof(size_t);	

//lol
size_t *root = NULL;
size_t *anti_root = NULL;


int mm_init(void)
{
    if (mem_heapsize() == 0){
	mem_init();
    }
    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */

/*
How does a block look?
Size|0 <- header
Next
Prev
...
Size|0 <- footer

the next points to the head of the next free block
prev points to the head of the previous free block
*/

void *mm_malloc(size_t size)
{
    //there are no empty blocks
    if(*root == NULL){
        int newsize = ALIGN(size + SIZE_T_SIZE);
        void *p = mem_sbrk(newsize);
        if (p == (void *)-1){ // error because bigger than max size of heap
	        return NULL;
        }
        return p;
    }
    //we iterate through a linked list 
    else {
        size_t *iter = root;
        while(*(iter + 1) != NULL){

            //there is space for a malloc
            if(GET_SIZE(iter) - 2 >= size){
                
                //case 1
                //the payload takes up- (almost) the entire free space
                if(GET_SIZE(iter) - size - 2 < 4){

                    //if there exists a previous block, we link
                    if(&iter != root){
                        PUT(*(iter + 2) + 1, iter);
                    } //otherwise, we have to change the root, as we deleted the first blok
                    else{
                        root = iter + 1;
                    }

                    //if there exists a next block, we link
                    if(&iter != anti_root){
                        PUT(*(iter + 1) + 2, iter);
                    } //otherwise, we have to change the antiroot, as we deleted the last block
                    else{
                        anti_root = iter + 2;
                    }
                }

                //case 2
                //after the payload, there is room for one more block
                else{
                    size_t *new_header;
                    new_header = iter + size + 2;

                    //if there exists a previous block, we link
                    if(&iter != root){
                        PUT(*(iter + 2) + 1, new_header);
                        PUT(new_header + 2, *(iter + 2));
                    } //otherwise, we have to change the root, as we have a new first block
                    else{
                        root = new_header + 1;
                    }

                    //if there exists a next block, we link
                    if(&iter != anti_root){
                        PUT(*(iter + 1) + 2, iter);
                        PUT(new_header + 1, *(iter + 1));
                    } //otherwise, we have to change the antiroot, as we have a new last block
                    else{
                        anti_root = new_header + 2;
                    }
                }

                return iter + 1;
            }
            else{
                iter = *(iter + 1);
                //pointer to the next free block
            }
        }
   }
   //nothing was found. in this case, we also need to extend.
    int newsize = ALIGN(size + SIZE_T_SIZE);
    void *p = mem_sbrk(newsize);
    if (p == (void *)-1){ // error because bigger than max size of heap
	    return NULL;
    }
    return p;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{

}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;
    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}














