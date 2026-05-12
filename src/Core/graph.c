#include "graph.h"

Graph* Graph_load(char* filename)
{
    FILE* fp;
    Graph* graph;
    int n, m, u, v;
    float p;

    // Open the input file.
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error: Failed to open file %s.\n", filename);
        return NULL;
    }

    // Read the number of nodes and arcs.
    (void)fscanf(fp, "%d %d", &n, &m);

    // Allocate memory for the graph.
    graph = Graph_create(n);

    // Read the arcs from the file and update the graph.
    for (int i = 0; i < m; i++) {
       (void) fscanf(fp, "%d %d %f", &u, &v, &p);
        Graph_set(graph, u, v, p);
    }

    // Close the input file and return the graph.
    fclose(fp);
    return graph;
}

/// @brief Sous-fonction de Graph_dfsPrint() pour les appels récursifs.
/// @param graph le graphe.
/// @param currID l'identifiant du noeud courant.
/// @param explored tableau de booléens indiquant pour chaque identifiant de
/// noeud s'il a été marqué comme ateint.
static void Graph_dfsPrintRec(Graph* graph, int currID, bool* explored)
{
    assert(graph);
    assert(explored);
    int size = 0;
    explored[currID] = true;
    printf("%d-", currID);
    Arc* arcsortant = Graph_getSuccessors(graph, currID, &size);
    for (int i = 0; i < size; i++)
    {
        if (!explored[arcsortant[i].target])
        {
            Graph_dfsPrintRec(graph, arcsortant[i].target, explored);
        }
    }
    free(arcsortant);

}

void Graph_dfsPrint(Graph* graph, int start)
{
    assert(graph);
    assert((start >= 0) && start < Graph_size(graph));
    bool* explored = (bool*)calloc(Graph_size(graph), sizeof(bool));
    Graph_dfsPrintRec(graph, start, explored);
    free(explored);
}

void Graph_bfsPrint(Graph* graph, int start)
{
    assert(graph);
    assert(start);
    int size = 0;
    bool* explored = (bool*)calloc(Graph_size(graph), sizeof(bool));
    assert(explored);

    IntList* queue = IntList_create();
    IntList_insertFirst(queue, start);
    explored[start] = true;
    while (!IntList_isEmpty(queue))
    {
        int currID = IntList_dequeue(queue);
        explored[currID] = true;
        printf("%d-", currID);
        Arc* arcsortant = Graph_getSuccessors(graph, currID, &size);
        for (int i = 0; i < size; i++)
        {
            if (!explored[arcsortant[i].target])
            {
                IntList_enqueue(queue, arcsortant[i].target);
                explored[arcsortant[i].target] = true;

            }
        }
        free(arcsortant);

    }
    IntList_destroy(queue);
    free(explored);

}

static IntTree* Graph_spanningTreeRec(Graph* graph, int current, bool* explored)
{
    assert(graph);
    assert(explored);
    int size = 0;
    explored[current] = true;
    IntTree* tree = IntTree_create(current);
    Arc* arcsortant = Graph_getSuccessors(graph, current, &size);
    for (int i = 0; i < size; i++)
    {
        if (!explored[arcsortant[i].target])
        {
            IntTree* subtree = Graph_spanningTreeRec(graph, arcsortant[i].target, explored);
            IntTree_addChild(tree, subtree);
        }
    }
    free(arcsortant);
    return tree;
}

IntTree* Graph_spanningTree(Graph* graph, int start)
{
    assert(graph);
    assert((start >= 0) && start < Graph_size(graph));
    bool* explored = (bool*)calloc(Graph_size(graph), sizeof(bool));
    IntTree* tree = Graph_spanningTreeRec(graph, start, explored);
    free(explored);
    return tree;

}
