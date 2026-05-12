#pragma once
#include "settings.h"

/// @brief Structure représentant un couple clé/valeur dans un dictionnaire.
typedef struct KVPair_s
{
    /// @brief Clé (chaine de caractères).
    char *key;

    /// @brief Pointeur vers la valeur.
    void *value;
} KVPair;

/// @brief Structure représentant un noeud dans un dictionnaire implémenté
/// à partir d'un AVL.
typedef struct DictNode_s
{
    /// @brief Pointeur vers le noeud parent.
    struct DictNode_s *parent;

    /// @brief Pointeur vers le fils gauche.
    struct DictNode_s *leftChild;

    /// @brief Pointeur vers le fils droit.
    struct DictNode_s *rightChild;

    /// @brief Hauteur du noeud dans l'AVL.
    int height;

    /// @brief Couple clé/valeur associé au noeud.
    KVPair pair;
} DictNode;

/// @brief Structure représentant un dictionnaire implémenté à partir d'un AVL.
typedef struct Dict_s
{
    /// @brief Racine de l'AVL associé.
    DictNode *root;

    /// @brief Taille du dictionnaire.
    /// Egalement le nombre de noeuds de l'AVL.
    int size;
} Dict;

/// @brief Créé un nouveau dictionnaire.
/// @return Le dictionnaire créé.
Dict *Dict_create(void);

/// @brief Détruit un dictionnaire créé avec Dict_create().
/// @param dict le dictionnaire.
void Dict_destroy(Dict *dict);

/// @brief Renvoie la taille d'un dictionnaire.
/// @param dict le dictionnaire.
/// @return La taille du dictionnaire.
int Dict_size(Dict *dict);

/// @brief Renvoie la valeur associée à une clé.
/// Si la clé n'est pas présente dans le dictionnaire,
/// cette fonction renvoie NULL.
/// @param dict le dictionnaire.
/// @param key la clé recherchée.
/// @return La valeur associée à la clé ou NULL si la clé n'est pas présente
/// dans le dictionnaire.
void *Dict_get(Dict *dict, char *key);

/// @brief Insert un couple clé/valeur dans un dictionnaire.
/// Si la clé est déjà présente, renvoie la valeur précédente.
/// Sinon, renvoie NULL.
/// @param dict le dictionnaire.
/// @param key la clé à insérer.
/// @param value la valeur associée à la clé.
/// @return La valeur précédente si la clé est déjà dans le dictionnaire,
/// NULL sinon.
void *Dict_insert(Dict *dict, char *key, void *value);

/// @brief Supprime une clé dans un dictionnaire.
/// Si la clé est présente, renvoie la valeur précédente.
/// Sinon, renvoie NULL.
/// @param dict le dictionnaire.
/// @param key la clé à supprimer.
/// @return La valeur précédente si la clé est dans le dictionnaire,
/// NULL sinon.
void *Dict_remove(Dict *dict, char *key);

/// @brief Structure représentant un itérateur pour un dictionnaire.
typedef struct DictIter_s
{
    Dict *dict;
    DictNode *curr;
    bool first;
} DictIter;

/// @brief Initialise un itérateur sur un dictionnaire.
/// @param dict le dictionnaire.
/// @param iter pointeur vers l'itérateur à initialiser.
void Dict_getIterator(Dict *dict, DictIter *iter);

/// @brief Renvoie le couple clé/valeur associé à la position d'un itérateur
/// puis avance l'itérateur.
/// @param iter l'itérateur.
/// @return Le couple clé/valeur sur lequel est l'itérateur.
KVPair *DictIter_next(DictIter *iter);

/// @brief Indique s'il reste des éléments à parcourir pour un itérateur.
/// @param iter l'itérateur.
/// @return false si l'itérateur a déjà parcouru tout le dictionnaire,
/// true sinon.
bool DictIter_hasNext(DictIter *iter);

// DEBUG

void Dict_print(Dict *dict);
void Dict_printTree(Dict *dict);
