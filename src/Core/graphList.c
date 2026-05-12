#include "graph.h"

#ifndef _GRAPH_MAT

typedef struct sGraph Graph;
typedef struct sGraphNode GraphNode;
typedef struct sArcList ArcList;

struct sGraph
{
    /// @brief Tableau contenant les noeuds du graphe.
    GraphNode* nodes;
    /// @brief Nombre de noeuds du graphe.
    int size;
};

/// @brief Structure représentant un noeud d'un graphe.
struct sGraphNode
{
    /// @brief Identifiant du noeud.
    /// Compris entre 0 et la taille du graphe moins 1.
    int id;
    /// @brief Degré entrant du noeud.
    int negativeValency;
    /// @brief Degré sortant du noeud.
    int positiveValency;
    /// @brief Liste des arcs sortants du noeud.
    ArcList* arcList;
};

/// @brief Structure représentant une liste simplement chaînée des arcs sortants d'un noeud.
struct sArcList
{
    /// @brief Pointeur vers l'élément suivant de la liste.
    /// Vaut NULL s'il s'agit du dernier élément.
    ArcList* next;
    /// @brief Arc associé au noeud de liste.
    Arc arc;
};

Graph* Graph_create(int size)
{
    Graph* graph = (Graph*)calloc(1, sizeof(Graph));
    if (!graph) return NULL;
    graph->size = size;
    graph->nodes = (GraphNode*)calloc(size, sizeof(GraphNode));
    assert(graph->nodes);
    for (int i = 0; i < size; i++)
    {
        graph->nodes[i].id = i;
    }

    return graph;
}

void Graph_destroy(Graph* graph)
{
    assert(graph);
    for (int i = 0; i < graph->size; i++)
    {
        ArcList* arcList = graph->nodes[i].arcList;
        while (arcList)
        {
            ArcList* next = arcList->next;
            free(arcList);
            arcList = next;
        }
    }

    free(graph->nodes);
    free(graph);
}

int Graph_size(Graph* graph)
{
    assert(graph);
    return graph->size;
}

void Graph_print(Graph* graph)
{
    // Parcours de tous les noeuds.
    for (int i = 0; i < graph->size; i++)
    {
        GraphNode* node = &graph->nodes[i];
        printf("Node %d [%d/%d] :", i, node->positiveValency, node->negativeValency);
       
        // Parcours de tous les arcs sortants.
        ArcList* arcList = node->arcList;
        while (arcList != NULL) {
            Arc arc = arcList->arc;
            printf("(%f, %d)", (double)arc.weight, arc.target);
            arcList = arcList->next;
        }
        printf("\n");
    }
}

void Graph_set(Graph* graph, int u, int v, float weight)
{
    // Vérification de la validité des identifiants de noeuds.

    assert(u >= 0 && u < graph->size);
    assert(v >= 0 && v < graph->size);

    ArcList* arcList = graph->nodes[u].arcList;
    ArcList *previous = NULL;
    while (arcList)
    {
        if (arcList->arc.target == v)
        {
            if (weight < 0.0f)
            {
                graph->nodes[u].positiveValency--;
                graph->nodes[v].negativeValency--;
                if (previous) previous->next = arcList->next;
                else graph->nodes[u].arcList = arcList->next;
                free(arcList);
            }
            else arcList->arc.weight = weight;
            return;
        }
        previous = arcList;
        arcList = arcList->next;
    }

    if (weight >= 0)
    {
        ArcList* newArcList = (ArcList*)calloc(1, sizeof(ArcList));
        assert(newArcList);
        newArcList->arc.source = u;
        newArcList->arc.target = v;
        newArcList->arc.weight = weight;
        newArcList->next = graph->nodes[u].arcList;
        graph->nodes[u].arcList = newArcList;
        graph->nodes[u].positiveValency++;
        graph->nodes[v].negativeValency++;
    }
}

float Graph_get(Graph* graph, int u, int v)
{
    GraphNode* node = &graph->nodes[u];
    ArcList* arcList = node->arcList;
    while (arcList != NULL) {
        Arc arc = arcList->arc;
        if (arc.target == v) {
            return arc.weight;
        }
        arcList = arcList->next;
    }
    return -1.0f;
}

int Graph_getPositiveValency(Graph* graph, int u)
{
    assert(graph);
    if ((u < 0) || (u > graph->size)) return -1;
    return graph->nodes[u].positiveValency;
}

int Graph_getNegativeValency(Graph* graph, int u)
{
    assert(graph);
    if ((u < 0) || (u > graph->size)) return -1;
    return graph->nodes[u].negativeValency;
}

Arc* Graph_getPredecessors(Graph* graph, int u, int* size)
{
    int count = 0;
    for (int i = 0; i < graph->size; i++) {
        GraphNode* node = &graph->nodes[i];
        ArcList* arcList = node->arcList;
        while (arcList != NULL) {
            Arc arc = arcList->arc;
            if (arc.target == u) {
                count++;
            }
            arcList = arcList->next;
        }
    }
    Arc* arcs = calloc(count, sizeof(Arc));
    assert(arcs);
    int index = 0;
    for (int i = 0; i < graph->size; i++) {
        GraphNode* node = &graph->nodes[i];
        ArcList* arcList = node->arcList;
        while (arcList) {
            Arc arc = arcList->arc;
            if (arc.target == u) 
            {
                arcs[index] = arc;
                index++;
                // printf("Predecessors of %d : (%d %d)\n",u, arcs[index].source, arcs[index].target);
            }
            arcList = arcList->next;
        }
    }
    if (count == 0)
    {
        *size = 0;
        free(arcs);
        return NULL;
    }

    *size = count;
    return arcs;
}

Arc* Graph_getSuccessors(Graph* graph, int u, int* size)
{
    assert(u >= 0 && u < graph->size);
    GraphNode* sourceNode = &graph->nodes[u];
    ArcList* arcList = sourceNode->arcList;
    if (arcList == NULL || sourceNode->positiveValency == 0)
    {
		*size = 0;
		return NULL;
	}

    Arc* successors = (Arc*)calloc(sourceNode->positiveValency, sizeof(Arc));
    assert(successors);

    int i = 0;
    while (arcList != NULL) 
    {
        successors[i] = arcList->arc;
        // printf("Successors of %d : (%d %d)\n", u, successors[i].source, successors[i].target);
         //printf("\n");
        i++;
        arcList = arcList->next;
    }

    *size = sourceNode->positiveValency;
    return successors;
}

#endif