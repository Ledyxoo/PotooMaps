#include "shortestPath.h"
#include "graph.h"
#include "binaryheap.h"
#include "path.h"
#include "intList.h"
#include "settings.h"
#include <math.h>


Path* Graph_shortestPath(Graph* graph, int start, int end)
{
    int nb_nodes = Graph_size(graph);
    if(start < 0 || start >= nb_nodes || end < 0 || end >= nb_nodes) return NULL;
    int* predecessors = calloc(1, nb_nodes * sizeof(int));
    float* distances = calloc(1, nb_nodes * sizeof(float));
    
    // Appliquer l'algorithme de Dijkstra pour trouver les chemins les plus courts
    Graph_dijkstra(graph, start, end, predecessors, distances);
    
    if (predecessors[end] == -1) 
    {
        free(predecessors);
        free(distances);
        return NULL;
    }
    
    // Reconstituer le chemin le plus court
    Path* path = Graph_dijkstraGetPath(predecessors, distances, end);
    
    free(predecessors);
    free(distances);
    
    return path;
}

void Graph_dijkstra(Graph* graph, int start, int end, int* predecessors, float* distances)
{
    (void)end;
    int size = Graph_size(graph);
    for (int i = 0; i < size; i++) {
        predecessors[i] = -1;
        distances[i] = INFINITY;
    }
    distances[start] = 0;
    BinaryHeap* heap = createBinaryHeap(size);
    insert(heap, start, 0);
    while (heap->size > 0) {
        HeapNode node = removeMin(heap);
        int source = node.vertex;
        if (distances[source] == INFINITY) break;
        int nbSucc;
        Arc* succ = Graph_getSuccessors(graph, source, &nbSucc);
        for (int i = 0; i < nbSucc; i++) {
            int target = succ[i].target;
            float weight = succ[i].weight + distances[source];
            if (weight < distances[target]) {
                distances[target] = weight;
                predecessors[target] = source;
                insert(heap, target, weight);
            }
        }
        free(succ);
    }
    freeBinaryHeap(heap);
}


Path* Graph_dijkstraGetPath(int* predecessors, float* distances, int end)
{
    Path* path = malloc(sizeof(Path));
    assert(path);
    if (predecessors[end] == -1)
    {
        path->list = NULL;
        path->distance = INFINITY;
        return path;
    }

    path->list = IntList_create();
    int current = end;
    while (current != -1)
    {
        IntList_insertFirst(path->list, current);
        current = predecessors[current];
    }
    path->distance = distances[end];

    return path;
}