#include "settings.h"
#include "intList.h"

IntList *IntList_create(void)
{
    IntList *list = (IntList *)calloc(1, sizeof(IntList));
    AssertNew(list);

    IntListNode *sentinel = &(list->sentinel);

    sentinel->next = sentinel;
    sentinel->prev = sentinel;
    sentinel->value = 0;

    list->nodeCount = 0;
    return list;
}

void IntList_destroy(IntList *list)
{
    if (list == NULL) return;

    IntListNode *sentinel = &(list->sentinel);
    IntListNode *curr = sentinel->next;
    while (curr != sentinel)
    {
        IntListNode *next = curr->next;
        free(curr);
        curr = next;
    }
    free(list);
}

bool IntList_isEmpty(IntList *list)
{
    assert(list);
    return (list->nodeCount <= 0);
}

void IntList_print(IntList *list)
{
    assert(list);

    IntListNode *sentinel = &(list->sentinel);
    IntListNode *curr = sentinel->next;
    printf("[");
    while (curr != sentinel->prev)
    {
        printf("%d,", curr->value);
        curr = curr->next;
    }
    if (curr != sentinel)
    {
        printf("%d", curr->value);
    }
    printf("]\n");
}

static void IntList_removeNode(IntList *list, IntListNode *node)
{
    assert(list->nodeCount > 0);
    assert(node != &(list->sentinel));

    IntListNode *prev = node->prev;
    IntListNode *next = node->next;
    prev->next = next;
    next->prev = prev;
    node->prev = node;
    node->next = node;

    list->nodeCount--;
}

static void IntList_insertNodeAfter(IntList *list, IntListNode *ref, IntListNode *node)
{
    assert(list && ref && node);

    IntListNode *next = ref->next;
    node->prev = ref;
    node->next = next;
    ref->next = node;
    next->prev = node;

    list->nodeCount++;
}

int IntList_getFirst(IntList *list)
{
    return list->sentinel.next->value;
}

int IntList_getLast(IntList *list)
{
    return list->sentinel.prev->value;
}

void IntList_insertFirst(IntList *list, int value)
{
    IntListNode *node = (IntListNode *)calloc(1, sizeof(IntListNode));
    AssertNew(node);
    node->value = value;
    IntList_insertNodeAfter(list, &(list->sentinel), node);
}

void IntList_insertLast(IntList *list, int value)
{
    IntListNode *node = (IntListNode *)calloc(1, sizeof(IntListNode));
    AssertNew(node);
    node->value = value;
    IntList_insertNodeAfter(list, list->sentinel.prev, node);
}

int IntList_popFirst(IntList *list)
{
    if (list->nodeCount <= 0)
    {
        assert(false);
        return 0;
    }
    IntListNode *node = list->sentinel.next;
    int value = node->value;
    IntList_removeNode(list, node);
    free(node);
    return value;
}

int IntList_popLast(IntList *list)
{
    if (list->nodeCount <= 0)
    {
        assert(false);
        return 0;
    }
    IntListNode *node = list->sentinel.prev;
    int value = node->value;
    IntList_removeNode(list, node);
    free(node);
    return value;
}

int IntList_isIn(IntList *list, int element)
{
    IntListNode *sentinel = &(list->sentinel);
    IntListNode *node = sentinel->next;
    while (node != sentinel)
    {
        if (node->value == element) return true;
        node = node->next;
    }
    return false;
}

IntList *IntList_copy(IntList *list)
{
    IntList *newList = IntList_create();

    IntListNode *sentinel = &(list->sentinel);
    IntListNode *node = sentinel->next;
    while (node != sentinel)
    {
        IntList_insertLast(newList, node->value);
        node = node->next;
    }

    return newList;
}

void IntList_concatenate(IntList *list1, IntList *list2)
{
    if (list2->nodeCount <= 0)
    {
        return;
    }

    list1->nodeCount += list2->nodeCount;

    IntListNode *last1 = list1->sentinel.prev;
    IntListNode *last2 = list2->sentinel.prev;
    IntListNode *first2 = list2->sentinel.next;
    IntListNode *sentinel1 = &(list1->sentinel);
    IntListNode *sentinel2 = &(list2->sentinel);

    last1->next = first2;
    first2->prev = last1;
    last2->next = sentinel1;
    sentinel1->prev = last2;

    sentinel2->next = sentinel2;
    sentinel2->prev = sentinel2;
}
