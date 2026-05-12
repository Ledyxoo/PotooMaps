#pragma once

#include "settings.h"
#include "graph.h"
#include "path.h"

/// @brief Renvoie un plus court chemin entre deux sommets d'un graphe.
/// Si aucun chemin n'existe, renvoie NULL.
/// Cette fonction suit l'algorithme de Dijkstra.
/// Elle a une complexité en O(n^2) où n désigne le nombre de noeuds du graphe.
/// 
/// @param graph le graphe.
/// @param start l'identifiant du sommet de départ.
/// @param end l'identifiant du sommet d'arrivée.
/// @return Un plus court chemin en cas d'existance, NULL sinon.
Path *Graph_shortestPath(Graph *graph, int start, int end);

/// @brief Effectue l'algorithme de Dijkstra.
/// Si end >= 0, cette fonction calcule un plus court chemin entre les noeuds
/// start et end.
/// Le chemin doit cependant être reconstruit à partir des tableaux
/// predecessors et distances en utilisant la fonction Graph_dijkstraGetPath().
/// Si end < 0, l'algorithme calcule un plus court chemin pour chaque noeud
/// accessible depuis start.
/// 
/// @param graph le graphe.
/// @param start l'identifiant du sommet de départ.
/// @param end l'identifiant du sommet d'arrivée ou un entier < 0 pour calculer
///     tous les chemins.
/// @param predecessors tableau péalablement alloué dont la taille est égale au
///     nombre de noeuds du graphe. Après l'appel à la fonction, il contient le
///     prédécesseur de chaque noeud.
/// @param distances tableau péalablement alloué dont la taille est égale au
///     nombre de noeuds du graphe. Après l'appel à la fonction, il contient
///     pour chaque noeud sa distance avec le noeud start.
void Graph_dijkstra(
    Graph *graph, int start, int end, int *predecessors, float *distances
);

/// @brief Reconstruit un chemin à partir de la sortie de l'algorithme de
/// Dijkstra.
/// 
/// @param predecessors tableau des prédecesseurs.
/// @param distances tableau des distances.
/// @param end noeud d'arrivée.
/// @return Le plus court chemin arrivant au noeud end s'il existe, NULL sinon.
Path *Graph_dijkstraGetPath(int *predecessors, float *distances, int end);

/// @brief Renvoie un plus court chemin entre deux sommets d'un graphe.
/// Si aucun chemin n'existe, renvoie NULL.
/// Cette fonction effectue une énumération complète de tous les chemins
/// possibles. Elle a une complexité en O(n!).
/// 
/// @param graph le graphe.
/// @param start l'identifiant du sommet de départ.
/// @param end l'identifiant du sommet d'arrivée.
/// @return Un plus court chemin en cas d'existance, NULL sinon.
Path *Graph_enumPath(Graph *graph, int start, int end);
