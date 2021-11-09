#ifndef BINARY_HEAP_H
#define BINARY_HEAP_H

#include <ace/types.h>
#include <ace/macros.h>

#define MAX_SIZE 25

typedef struct {
    union {
        UBYTE ubSize;
        tUbCoordYX _alignment;
    };
    tUbCoordYX p_uwAry[MAX_SIZE];
    UBYTE 
} tBinaryHeap;

#define bhParent(i) (((i) - 1) >> 1)

#define bhLeftChild(i) (((i) << 1) + 1)

#define bhRightChild(i) ((i << 1) + 2)

#define bhSwap(x, y) do {   \
    tUbCoordYX temp = x;    \
    x = y;                  \
    y = temp;               \
} while (0)

void bhMaxHeapify(tBinaryHeap heap, unsigned char i);

/**
 * Insert the int data at the appropriate position into the heap
 */
#define insert(heap, data) do {                 \
    UBYTE n = heap.ubSize;                      \
    if (n < MAX_SIZE) {                         \
        /* first insert the time at the last    \
           position of the array and move it    \
           up */                                \
        tUbCoordYX *a = heap.p_uwAry;           \
        a[n] = data;                            \
        heap.ubSize = n + 1;                    \
        /* move up until the heap property      \
           satisfies */                         \
        UBYTE parentIdx = bhParent(n);          \
        while (n != 0 &&                        \
               a[parentIdx].uwYX < a[n].uwYX) { \
            bhSwap(a[parentIdx], a[n]);         \
            n = parentIdx;                      \
            parentIdx = bhParent(n);            \
        }                                       \
    }                                           \
} while (0)

/**
 * returns the maximum item of the heap
 */
#define get_max(heap) (heap).p_usAry[0]

/**
 * Delete the max item and return it
 */
#define pop_max(heap) do {                          \
    int max_item = heap.p_usAry[0];                 \
    /* replace the first item with the last item */ \
    heap.ubSize = heap.ubSize - 1;                  \
    heap.p_usAry[0] = heap.p_usAry[heap.ubSize];    \
    /* maintain the heap property by heapifying the \ 
    first item */                                   \
    max_heapify(heap, 0);                           \
    return max_item;                                \
} while (0)

#endif
