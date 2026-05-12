#include "binaryheap.h"

BinaryHeap* createBinaryHeap(int capacity) {
    BinaryHeap* heap = calloc(1, sizeof(BinaryHeap));
    AssertNew(heap);
    heap->heapArray = calloc(capacity, sizeof(HeapNode));
    heap->size = 0;
    heap->capacity = capacity;
    return heap;
}

static void swap(HeapNode* a, HeapNode* b) {
    HeapNode temp = *a;
    *a = *b;
    *b = temp;
}

static void heapifyUp(BinaryHeap* heap, int index) {
    int parent = (index - 1) / 2;
    if (index > 0 && heap->heapArray[parent].distance > heap->heapArray[index].distance) {
        swap(&heap->heapArray[parent], &heap->heapArray[index]);
        heapifyUp(heap, parent);
    }
}

static void heapifyDown(BinaryHeap* heap, int index) {
    int left = 2 * index + 1;
    int right = 2 * index + 2;
    int smallest = index;

    if (left < heap->size && heap->heapArray[left].distance < heap->heapArray[smallest].distance)
        smallest = left;
    if (right < heap->size && heap->heapArray[right].distance < heap->heapArray[smallest].distance)
        smallest = right;

    if (smallest != index) {
        swap(&heap->heapArray[index], &heap->heapArray[smallest]);
        heapifyDown(heap, smallest);
    }
}

static void resizeHeap(BinaryHeap* heap) {
    heap->capacity *= 2;
    heap->heapArray = realloc(heap->heapArray, heap->capacity * sizeof(HeapNode));
    AssertNew(heap->heapArray);
}

void insert(BinaryHeap* heap, int vertex, float distance) {
    if (heap->size == heap->capacity) {
        resizeHeap(heap);
    }

    HeapNode node;
    node.vertex = vertex;
    node.distance = distance;

    heap->heapArray[heap->size] = node;
    heapifyUp(heap, heap->size);
    heap->size++;
}

HeapNode removeMin(BinaryHeap* heap) {
    HeapNode min = heap->heapArray[0];
    heap->heapArray[0] = heap->heapArray[heap->size - 1];
    heap->size--;
    heapifyDown(heap, 0);
    return min;
}

void printHeap(BinaryHeap* heap) {
    printf("Heap: ");
    for (int i = 0; i < heap->size; i++) {
        printf("(%d, %.2f) ", heap->heapArray[i].vertex, (double)heap->heapArray[i].distance);
    }
    printf("\n");
}

void freeBinaryHeap(BinaryHeap* heap) {
    free(heap->heapArray);
    free(heap);
}