#pragma once

#include "settings.h"

typedef struct sIntList IntList;
typedef struct sIntListNode IntListNode;

/// @brief Structure représentant un noeud d'une liste d'entiers.
struct sIntListNode
{
    /// @brief Pointeur vers le noeud précédent.
    IntListNode *prev;

    /// @brief Pointeur vers le noeud suivant.
    IntListNode *next;

    /// @brief Valeur du noeud.
    int value;
};

/// @brief Structure représentant une liste doublement chaînée d'entiers.
typedef struct sIntList
{
    /// @brief Sentinelle.
    IntListNode sentinel;

    /// @brief Le nombre de noeuds de la liste.
    int nodeCount;
} IntList;

/// @brief Crée une liste vide.
/// Cette liste peut notamment être utilisée comme une pile ou comme une file.
/// @return La liste créée.
IntList *IntList_create(void);

/// @brief Détruit une liste d'entiers.
/// @param list la liste.
void IntList_destroy(IntList *list);

/// @brief Indique si une liste d'entiers est vide.
/// @param list la liste.
/// @return true si la liste est vide, false sinon.
bool IntList_isEmpty(IntList *list);

/// @brief Affiche une liste d'entiers.
/// @param list la liste.
void IntList_print(IntList *list);

/// @brief Renvoie le premier entier d'une liste.
/// L'utilisateur doit au préalable vérifier que le liste est non vide.
/// @param list la liste.
/// @return Le premier entier de la liste.
int IntList_getFirst(IntList *list);

/// @brief Renvoie le dernier entier d'une liste.
/// L'utilisateur doit au préalable vérifier que le liste est non vide.
/// @param list la liste.
/// @return Le premier dernier de la liste.
int IntList_getLast(IntList *list);

/// @brief Insert un entier au début d'une liste.
/// @param list la liste.
/// @param value l'entier à insérer.
void IntList_insertFirst(IntList *list, int value);

/// @brief Insert un entier à la fin d'une liste.
/// @param list la liste.
/// @param value l'entier à insérer.
void IntList_insertLast(IntList *list, int value);

/// @brief Supprime et renvoie le premier entier d'une liste.
/// L'utilisateur doit au préalable vérifier que le liste est non vide.
/// @param list la liste.
/// @return Le premier entier de la liste.
int IntList_popFirst(IntList *list);

/// @brief Supprime et renvoie le dernier entier d'une liste.
/// L'utilisateur doit au préalable vérifier que le liste est non vide.
/// @param list la liste.
/// @return Le dernier entier de la liste.
int IntList_popLast(IntList *list);

/// @brief Enfile un entier dans une file.
/// @param list la liste utilisée comme une file.
/// @param value l'entier.
INLINE void IntList_enqueue(IntList *list, int value)
{
    IntList_insertLast(list, value);
}

/// @brief Défile un entier dans une file.
/// @param list la liste utilisée comme une file.
/// @param value l'entier.
INLINE int IntList_dequeue(IntList *list)
{
    return IntList_popFirst(list);
}

/// @brief Empile un entier dans une pile.
/// @param list la liste utilisée comme une pile.
/// @param value l'entier.
INLINE void IntList_push(IntList *list, int value)
{
    IntList_insertLast(list, value);
}

/// @brief Dépile un entier dans une pile.
/// @param list la liste utilisée comme une pile.
/// @param value l'entier.
INLINE int IntList_pop(IntList *list)
{
    return IntList_popLast(list);
}

/// @brief Teste la présence d'un élément dans une liste.
/// @param list la liste.
/// @param element l'élément recherché.
/// @return true si l'élément est présent dans la liste,
/// false sinon.
int IntList_isIn(IntList *list, int element);

/// @brief Copie une liste avec tous ses noeuds.
/// @param list la liste à copier.
/// @return Une copie profonde de la liste.
IntList *IntList_copy(IntList *list);

/// @brief Concatène deux listes.
/// Les noeuds de la deuxième liste sont ajoutés en fin de la première liste.
/// La seconde liste devient vide après l'appel à cette fonction.
/// Cette méthode s'exécute en temps constant.
/// @param list1 la première liste.
/// @param list2 la seconde liste.
void IntList_concatenate(IntList *list1, IntList *list2);
