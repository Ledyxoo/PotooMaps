#pragma once

#include "settings.h"
#include "intList.h"

/// @brief Structure représentant un chemin dans un graphe.
typedef struct Path_s
{
    /// @brief Liste des sommets du chemin.
    /// Le premier élément de la liste est le sommet et départ
    /// et le dernier élément le sommet d'arrivée.
    IntList *list;

    /// @brief Longueur du chemin.
    /// Autrement dit la somme des poids des arcs qui composent le chemin.
    float distance;
} Path;

/// @brief Crée un nouveau chemin.
/// @param start le sommet de départ du chemin.
/// @return Le chemin créé.
Path *Path_create(int start);

/// @brief Détruit un chemin créé avec Path_create().
/// @param path le chemin à détruire.
void Path_destroy(Path *path);

/// @brief Copie un chemin.
/// Il s'agit d'une copie profonde, tous les noeuds de la liste sont réalloués.
/// @param path le chemin.
/// @return Une copie profonde du chemin.
Path *Path_copy(Path *path);

/// @brief Insert un sommet au début d'un chemin.
/// @param path le chemin.
/// @param nodeID l'identifiant du noeud à ajouter.
/// @param weight le poids le l'arc ajouté.
void Path_insertFirst(Path *path, int nodeID, float weight);

/// @brief Insert un sommet à la fin d'un chemin.
/// @param path le chemin.
/// @param nodeID l'identifiant du noeud à ajouter.
/// @param weight le poids le l'arc ajouté.
void Path_insertLast(Path *path, int nodeID, float weight);

/// @brief Affiche un chemin.
/// @param path le chemin.
void Path_print(Path *path);
