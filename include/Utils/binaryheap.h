
#include "settings.h"

typedef struct {
    int vertex;
    float distance;
} HeapNode;

typedef struct {
    HeapNode* heapArray;
    int size;
    int capacity;
} BinaryHeap;

BinaryHeap* createBinaryHeap(int capacity);
void insert(BinaryHeap* heap, int vertex, float distance);
HeapNode removeMin(BinaryHeap* heap);
void printHeap(BinaryHeap* heap);
void freeBinaryHeap(BinaryHeap* heap);