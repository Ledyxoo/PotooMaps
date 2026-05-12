#pragma once

#include "settings.h"
#include "intTree.h"


//#define _GRAPH_MAT

/// @brief Structure représentant un arc dans un graphe.
typedef struct sArc
{
    /// @brief Identifiant du noeud de départ.
    int source;

    /// @brief Identifiant du noeud d'arrivée.
    int target;

    /// @brief Poids de l'arc.
    float weight;
} Arc;

//------------------------------------------------------------------------------
//  Fonctions dépendantes de l'implémentation

/// @brief Structure représentant un graphe orienté.
/// Deux implémentations sont disponibles, soit avec une matrice d'adjacence,
/// soit avec une liste d'adjacence.
typedef struct sGraph Graph;

/// @brief Crée un nouveau graphe.
/// Le nombre de noeuds doit être défini au moment de la création et ne peut
/// plus être modifié par la suite.
/// @param size Le nombre de noeuds du graphe.
/// @return Le graphe créé.
Graph *Graph_create(int size);

/// @brief Détruit un graphe créé avec Graph_create().
/// @param graph le graphe.
void Graph_destroy(Graph *graph);

/// @brief Renvoie le nombre de noeuds d'un graphe.
/// @param graph le graphe.
/// @return Le nombre de noeuds du graphe.
int Graph_size(Graph *graph);

/// @brief Affiche les données un graphe.
/// @param graph le graphe.
void Graph_print(Graph *graph);

/// @brief Définit le poids d'un arc dans un graphe.
/// Cette méthode permet également de supprimer un arc en donnant un poids
/// négatif.
/// @param graph le graphe
/// @param u l'identifiant du noeud de départ.
/// @param v l'identifiant du noeud d'arrivée.
/// @param weight le poids de l'arc.
void Graph_set(Graph *graph, int u, int v, float weight);

/// @brief Renvoie le poids d'un arc dans un graphe.
/// S'il n'y a pas d'arc, cette méthode renvoie -1.0f.
/// @param graph le graphe.
/// @param u l'identifiant du noeud de départ.
/// @param v l'identifiant du noeud d'arrivée.
/// @return Le poids de l'arc partant de u et arrivant à v
/// ou -1.0f s'il n'y a pas d'arc.
float Graph_get(Graph *graph, int u, int v);

/// @brief Renvoie le nombre d'arc partant d'un noeud dans un graphe.
/// @param graph le graphe.
/// @param u l'identifiant du noeud de départ.
/// @return Le nombre d'arcs partant du noeud u.
int Graph_getPositiveValency(Graph *graph, int u);

/// @brief Renvoie le nombre d'arc arrivant à un noeud dans un graphe.
/// @param graph le graphe.
/// @param u l'identifiant du noeud d'arrivée.
/// @return Le nombre d'arcs arrivant au noeud u.
int Graph_getNegativeValency(Graph *graph, int u);

/// @brief Renvoie le tableau des prédécesseurs d'un noeud dans un graphe.
/// @param graph le graphe.
/// @param u l'identifiant du noeud d'arrivée.
/// @param[out] size adresse de l'entier où écrire la taille du tableau.
/// @return Le tableau contenant les arcs arrivant au noeud u.
Arc *Graph_getPredecessors(Graph *graph, int u, int *size);

/// @brief Renvoie le tableau des successeurs d'un noeud dans un graphe.
/// @param graph le graphe.
/// @param u l'identifiant du noeud de départ.
/// @param[out] size adresse de l'entier où écrire la taille du tableau.
/// @return Le tableau contenant les arcs partant du noeud u.
Arc *Graph_getSuccessors(Graph *graph, int u, int *size);

//------------------------------------------------------------------------------
//  Fonctions communes

/// @brief Charge un graphe depuis un fichier.
/// @param filename chemin du fichier.
/// @return Le graphe correspondant.
Graph *Graph_load(char *filename);

/// @brief Affiche les noeuds d'un graphe suivant une exploration en profondeur.
/// @param graph le graphe.
/// @param start l'identifiant du noeud de départ.
void Graph_dfsPrint(Graph* graph, int start);

/// @brief Affiche les noeuds d'un graphe suivant une exploration en largeur.
/// @param graph le graphe.
/// @param start l'identifiant du noeud de départ.
void Graph_bfsPrint(Graph* graph, int start);

/// @brief Renvoie un arbre couvrant d'un graphe obtenu avec un parcours en
/// profondeur.
/// @param graph le graphe.
/// @param start l'identifiant du noeud étant à la racine de l'arbre.
/// @return L'arbre couvrant du graphe.
IntTree* Graph_spanningTree(Graph* graph, int start);
