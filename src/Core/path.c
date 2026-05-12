#include "path.h"

Path *Path_create(int start)
{
    Path *path = (Path *)calloc(1, sizeof(Path));
    AssertNew(path);

    path->distance = 0.0f;
    path->list = IntList_create();
    IntList_insertLast(path->list, start);

    return path;
}

void Path_destroy(Path *path)
{
    if (path == NULL) return;

    IntList_destroy(path->list);
    free(path);
}

Path *Path_copy(Path *path)
{
    assert(path && path->list);

    Path *newPath = (Path *)calloc(1, sizeof(Path));
    AssertNew(newPath);

    newPath->distance = path->distance;
    newPath->list = IntList_copy(path->list);

    return newPath;
}

void Path_insertFirst(Path *path, int nodeID, float weight)
{
    assert(path && path->list);

    path->distance += weight;
    IntList_insertFirst(path->list, nodeID);
}

void Path_insertLast(Path *path, int nodeID, float weight)
{
    assert(path && path->list);

    path->distance += weight;
    IntList_insertLast(path->list, nodeID);
}

void Path_print(Path *path)
{
    if (path == NULL)
    {
        printf("path : NULL\n");
        return;
    }

    printf("path (distance = %f) : ", (double)path->distance);
    IntList_print(path->list);
}
