#pragma once

#include "settings.h"
#include "intList.h"

/// @brief Structure représentant un arbre d'entiers.
/// Chaque noeud peut avec un nombre quelconque d'enfants.
typedef struct sIntTree
{
    /// @brief Pointeur vers le premier enfants du noeud.
    struct sIntTree* firstChild;

    /// @brief Pointeur vers le noeud frère suivant.
    struct sIntTree* nextSibling;

    /// @brief Valeur du noeud.
    int value;
} IntTree;

/// @brief Crée un arbre avec uniquement un noeud.
/// @param value la valeur du noeud (la racine de l'arbre).
/// @return L'arbre créé.
IntTree* IntTree_create(int value);

/// @brief Détruit récursivement tous les noeuds d'un arbre donné par sa racine.
/// @param tree (la racine de) l'arbre.
void IntTree_destroy(IntTree* tree);

/// @brief Ajoute un enfant à un noeud d'un arbre.
/// @param node le noeud.
/// @param child l'enfant à ajouter. Il peut s'agir d'un arbre complet.
void IntTree_addChild(IntTree* node, IntTree* child);

/// @brief Recherche un noeud dans un arbre.
/// @param node la racine de l'arbre.
/// @param value la valeur recherchée.
/// @return Un noeud ayant la valeur recherchée ou NULL si aucun noeud ne
/// possède cette valeur.
IntTree* IntTree_get(IntTree* node, int value);

/// @brief Renvoie la liste des valeurs des noeuds d'un arbre.
/// @param tree l'arbre.
/// @return La liste des valeurs des noeuds de l'arbre.
IntList* IntTree_values(IntTree* tree);

/// @brief Renvoie la liste des valeurs des feuilles d'un arbre.
/// @param tree l'arbre.
/// @return La liste des valeurs des feuilles de l'arbre.
IntList* IntTree_leaves(IntTree* tree);

/// @brief Affiche un arbre
/// @param arbre l'arbre.
void IntTree_print(IntTree* arbre);
