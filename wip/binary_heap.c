#include "binary_heap.h"

/** 
 * Moves the item at position i of array a into its appropriate position
 */
void bhMaxHeapify(tBinaryHeap heap, unsigned char i) {
    // find left child node
    UBYTE left = bhLeftChild(i);
    // find right child node
    UBYTE right = bhRightChild(i);
    // find the largest among 3 nodes
    UBYTE largest = i;
    UBYTE n = heap.ubSize;
    tUbCoordYX *a = heap.p_uwAry;
    // check if the left node is larger than the current node
    if (left <= n && a[left].uwYX > a[largest].uwYX) {
        largest = left;
    }
    // check if the right node is larger than the current node
    if (right <= n && a[right].uwYX > a[largest].uwYX) {
        largest = right;
    }
    // swap the largest node with the current node 
    // and repeat this process until the current node is larger than 
    // the right and the left node
    if (largest != i) {
        bhSwap(a[i], a[largest]);
        bhMaxHeapify(heap, largest);
    }
}
