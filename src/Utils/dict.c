#include "dict.h"

static bool Dict_find(Dict* dict, char* key, DictNode** res);
static void Dict_balance(Dict *dict, DictNode *node);

static DictNode *DictNode_create(char *key, void *value)
{
    DictNode *node = (DictNode *)calloc(1, sizeof(DictNode));
    AssertNew(node);

    node->pair.key = Strdup(key);
    node->pair.value = value;

    return node;
}

static void DictNode_destroy(DictNode *node)
{
    if (!node) return;
    free(node->pair.key);
    free(node);
}

static void DictNode_destroyRec(DictNode *node)
{
    if (!node) return;
    DictNode_destroyRec(node->leftChild);
    DictNode_destroyRec(node->rightChild);
    DictNode_destroy(node);
}

static void DictNode_setLeft(DictNode *node, DictNode *left)
{
    node->leftChild = left;
    if (left != NULL)
        left->parent = node;
}

static void DictNode_setRight(DictNode *node, DictNode *right)
{
    node->rightChild = right;
    if (right != NULL)
        right->parent = node;
}

static bool DictNode_update(DictNode *node)
{
    if (node == NULL) return false;

    int hr = (node->rightChild) ? node->rightChild->height : -1;
    int hl = (node->leftChild) ? node->leftChild->height : -1;
    int h = (hr > hl) ? hr + 1 : hl + 1;

    if (node->height != h)
    {
        node->height = h;
        return true;
    }
    return false;
}

static int DictNode_getBalance(DictNode *node)
{
    if (node == NULL) return 0;

    int hr = (node->rightChild) ? node->rightChild->height : -1;
    int hl = (node->leftChild) ? node->leftChild->height : -1;
    return hr - hl;
}

static DictNode *DictNode_maximum(DictNode *node)
{
    assert(node != NULL);

    while (node->rightChild != NULL)
    {
        node = node->rightChild;
    }

    return node;
}

Dict *Dict_create(void)
{
    Dict *dict = (Dict *)calloc(1, sizeof(Dict));
    AssertNew(dict);

    return dict;
}

void Dict_destroy(Dict *dict)
{
    if (!dict) return;

    DictNode_destroyRec(dict->root);
    free(dict);
}

int Dict_size(Dict *dict)
{
    assert(dict);
    return dict->size;
}

static void DictNode_print(DictNode *node)
{
    if (!node) return;
    DictNode_print(node->leftChild);
    printf("  \"%s\": %p,\n", node->pair.key, node->pair.value);
    DictNode_print(node->rightChild);
}

void Dict_print(Dict *dict)
{
    printf("size = %d\n", dict->size);
    DictNode_print(dict->root);
}

static void DictNode_printTree(DictNode *node, int depth, int isLeft)
{
    if (!node) return;

    DictNode_printTree(node->rightChild, depth + 1, false);

    for (int i = 0; i < depth - 1; i++) printf("    ");
    if (depth > 0)
    {
        if (isLeft) printf("  \\-");
        else printf("  /-");
    }
    printf("[%s]\n", node->pair.key);

    DictNode_printTree(node->leftChild, depth + 1, true);
}

void Dict_printTree(Dict *dict)
{
    DictNode_printTree(dict->root, 0, false);
}

void *Dict_insert(Dict *dict, char *key, void *value)
{
    if (dict->root == NULL)
    {
        dict->root = DictNode_create(key, value);
        dict->size = 1;

        return NULL;
    }

    DictNode *node = NULL;
    if (Dict_find(dict, key, &node))
    {
        // La donnée est déjà présente dans l'arbre
        void *prevValue = node->pair.value;
        node->pair.value = value;

        return prevValue;
    }

    // Crée le nouveau noeud
    DictNode *newNode = DictNode_create(key, value);
    if (strcmp(key, node->pair.key) < 0)
    {
        DictNode_setLeft(node, newNode);
    }
    else
    {
        DictNode_setRight(node, newNode);
    }
    dict->size++;

    // Equilibre l'arbre à partir du parent du nouveau noeud
    Dict_balance(dict, node);

    return NULL;
}

static bool Dict_find(Dict *dict, char *key, DictNode **res)
{
    if (dict->root == NULL)
    {
        *res = NULL;
        return false;
    }

    DictNode *node = dict->root;
    while (true)
    {
        int cmp = strcmp(key, node->pair.key);
        if (cmp == 0)
        {
            *res = node;
            return true;
        }
        else if (cmp < 0)
        {
            if (node->leftChild == NULL)
            {
                *res = node;
                return false;
            }
            node = node->leftChild;
        }
        else
        {
            if (node->rightChild == NULL)
            {
                *res = node;
                return false;
            }
            node = node->rightChild;
        }
    }
}

void *Dict_get(Dict *dict, char *key)
{
    DictNode *res = NULL;
    if (Dict_find(dict, key, &res))
    {
        return res->pair.value;
    }
    return NULL;
}

static void Dict_replaceChild(
    Dict *dict, DictNode *parent,
    DictNode *curChild, DictNode *newChild)
{
    if (parent == NULL)
    {
        dict->root = newChild;
    }
    else
    {
        if (parent->leftChild == curChild)
            parent->leftChild = newChild;
        else if (parent->rightChild == curChild)
            parent->rightChild = newChild;
        else
            assert(false);
    }

    if (newChild)
        newChild->parent = parent;
}

static void Dict_rotateLeft(Dict *dict, DictNode *node)
{
    if (node == NULL || node->rightChild == NULL) return;

    DictNode *parent = node->parent;
    DictNode *rightChild = node->rightChild;

    DictNode_setRight(node, rightChild->leftChild);
    DictNode_setLeft(rightChild, node);
    Dict_replaceChild(dict, parent, node, rightChild);

    // Met à jour les hauteurs
    DictNode_update(node);
    DictNode_update(rightChild);
}

static void Dict_rotateRight(Dict *dict, DictNode *node)
{
    if (node == NULL || node->leftChild == NULL) return;

    DictNode *parent = node->parent;
    DictNode *leftChild = node->leftChild;

    DictNode_setLeft(node, leftChild->rightChild);
    DictNode_setRight(leftChild, node);
    Dict_replaceChild(dict, parent, node, leftChild);

    // Met à jour les hauteurs
    DictNode_update(node);
    DictNode_update(leftChild);
}

static void Dict_balance(Dict *dict, DictNode *node)
{
    // Remonte éventuellement jusqu'à la racine pour rééquilibrer l'arbre
    while (node != NULL)
    {
        bool updated = DictNode_update(node);
        int balance = DictNode_getBalance(node);
        if (!updated && (abs(balance) < 2))
        {
            // Le noeud est à jour
            return;
        }

        DictNode *parent = node->parent;

        if (balance == 2)
        {
            int rightBalance = DictNode_getBalance(node->rightChild);
            if (rightBalance == -1)
            {
                Dict_rotateRight(dict, node->rightChild);
            }
            Dict_rotateLeft(dict, node);
        }
        else if (balance == -2)
        {
            int leftBalance = DictNode_getBalance(node->leftChild);
            if (leftBalance == 1)
            {
                Dict_rotateLeft(dict, node->leftChild);
            }
            Dict_rotateRight(dict, node);
        }

        node = parent;
    }
}

void *Dict_remove(Dict *dict, char *key)
{
    if (dict->root == NULL)
    {
        return NULL;
    }

    DictNode *node = NULL;
    if (Dict_find(dict, key, &node) == false)
    {
        return NULL;
    }
    assert(node != NULL);
    void *value = node->pair.value;

    DictNode *start = NULL;
    if (node->leftChild == NULL)
    {
        // Remplacement par le fils droit
        Dict_replaceChild(dict, node->parent, node, node->rightChild);
        start = node->parent;
        DictNode_destroy(node);
    }
    else if (node->rightChild == NULL)
    {
        // Remplacement par le fils gauche
        Dict_replaceChild(dict, node->parent, node, node->leftChild);
        start = node->parent;
        DictNode_destroy(node);
    }
    else
    {
        // Le noeud a deux fils
        // On l'échange avec sa valeur immédiatement inférieure (qui n'a plus de fils droit)
        DictNode *maxLeft = DictNode_maximum(node->leftChild);
        KVPair pair = node->pair;
        node->pair = maxLeft->pair;
        maxLeft->pair = pair;

        // Puis on le supprime comme précédemment
        Dict_replaceChild(dict, maxLeft->parent, maxLeft, maxLeft->leftChild);
        start = maxLeft->parent;
        DictNode_destroy(maxLeft);
    }
    dict->size--;

    // Equilibre l'arbre à partir du parent du noeud supprimé
    Dict_balance(dict, start);

    return value;
}

void Dict_getIterator(Dict *dict, DictIter *iter)
{
    DictNode *curr = NULL;
    if (dict->root)
    {
        curr = dict->root;
        while (curr->leftChild)
        {
            curr = curr->leftChild;
        }
    }

    iter->curr = curr;
    iter->dict = dict;
}

KVPair *DictIter_next(DictIter *iter)
{
    if (iter->curr == NULL)
    {
        return NULL;
    }

    DictNode *curr = iter->curr;
    DictNode *next = NULL;

    KVPair *pair = &(curr->pair);

    if (curr->rightChild != NULL)
    {
        // On cherche la valeur suivante en descendant dans l'arbre
        next = curr->rightChild;
        while (next->leftChild)
            next = next->leftChild;
    }
    else
    {
        // On cherche la valeur suivante en remontant dans l'arbre
        next = curr->parent;
        while (next)
        {
            if (curr == next->leftChild)
                break;

            curr = next;
            next = next->parent;
        }
    }

    iter->curr = next;

    return pair;
}

bool DictIter_hasNext(DictIter *iter)
{
    return iter->curr != NULL;
}