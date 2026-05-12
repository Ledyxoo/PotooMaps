#include "settings.h"
#include "intTree.h"

IntTree *IntTree_create(int value)
{
    IntTree *node = (IntTree *)calloc(1, sizeof(IntTree));
    AssertNew(node);
    node->value = value;
    return node;
}

void IntTree_destroy(IntTree *tree)
{
    if (!tree) return;
    IntTree_destroy(tree->firstChild);
    IntTree_destroy(tree->nextSibling);
    free(tree);
}

void IntTree_addChild(IntTree *node, IntTree *child)
{
    if ((!node) || (!child)) return;
    child->nextSibling = node->firstChild;
    node->firstChild = child;
}

IntTree *IntTree_get(IntTree *node, int value)
{
    if ((!node) || (node->value == value)) return node;

    IntTree *res = IntTree_get(node->firstChild, value);
    if (res) return res;

    res = IntTree_get(node->nextSibling, value);
    if (res) return res;

    return NULL;
}

static void IntTree_valuesRec(IntTree *currNode, IntList *list)
{
    if (!currNode) return;

    IntList_insertLast(list, currNode->value);
    IntTree_valuesRec(currNode->firstChild, list);
    IntTree_valuesRec(currNode->nextSibling, list);
}

IntList *IntTree_values(IntTree *tree)
{
    assert(tree);

    IntList *list = IntList_create();
    IntTree_valuesRec(tree, list);
    return list;
}

static void IntTree_leavesRec(IntTree *currNode, IntList *list)
{
    if (!currNode) return;

    if (currNode->firstChild)
    {
        IntTree_leavesRec(currNode->firstChild, list);
    }
    else
    {
        IntList_insertLast(list, currNode->value);
    }
    IntTree_leavesRec(currNode->nextSibling, list);
}

IntList *IntTree_leaves(IntTree *tree)
{
    assert(tree);

    IntList *list = IntList_create();
    IntTree_leavesRec(tree, list);
    return list;
}

static int numberLen(int value)
{
    int res = 1;
    while (value >= 10)
    {
        value /= 10;
        res++;
    }
    return res;
}

static int maxNumberLen(IntTree *node)
{
    if (!node) return 0;
    int score = numberLen(node->value);
    int score_ = maxNumberLen(node->firstChild);
    if (score < score_) score = score_;
    score_ = maxNumberLen(node->nextSibling);
    if (score < score_) score = score_;
    return score;
}

static int print(IntTree *noeud, int level, int ligne, char **map, int maxLen)
{
    if (!noeud) return 0;
    int gauche = print(noeud->firstChild, level + 1, ligne, map, maxLen);
    char buffer[1024];
    int len = numberLen(noeud->value);
    int i = 0;
    for (i = 0; i < maxLen - len; i++) buffer[i] = '-';
    if ((noeud->firstChild) && (noeud->firstChild->nextSibling)) sprintf(buffer + i, "[%d]+", noeud->value);
    else sprintf(buffer + i, "[%d] ", noeud->value);
    for (int col = 0; col < maxLen + 3; col++) map[ligne][level * (maxLen + 3) + col] = buffer[col];
    int nbLignes = (gauche > 0) ? gauche : 1;
    int ligne_ = ligne;
    if ((noeud->firstChild == NULL) || (noeud->firstChild->nextSibling == NULL))
    {
        if (noeud->firstChild) map[ligne][level * (maxLen + 3) + maxLen + 2] = '-';
        return nbLignes;
    }
    IntTree *current = noeud->firstChild->nextSibling;
    while (current)
    {
        for (int j = ligne_ + 1; j <= ligne + nbLignes; j++)
        {
            map[j][level * (maxLen + 3) + maxLen + 2] = '|';
        }
        map[ligne + nbLignes + 1][level * (maxLen + 3) + maxLen + 2] = '+';
        ligne_ = ligne + nbLignes + 1;
        nbLignes += print(current, level + 1, ligne + nbLignes + 1, map, maxLen) + 1;
        current = current->nextSibling;
    }
    return nbLignes;
}

void IntTree_print(IntTree *arbre)
{
    char **map = (char **)calloc(200, sizeof(char *));
    for (int i = 0; i < 200; i++)
    {
        map[i] = (char *)calloc(1024, sizeof(char));
        for (int j = 0; j < 1023; j++) map[i][j] = ' ';
    }
    int maxLen = maxNumberLen(arbre);
    print(arbre, 0, 0, map, maxLen);
    int fin = 0;
    for (int i = 0; i < 200; i++)
    {
        if (fin)
        {
            free(map[i]);
            continue;
        }
        fin = 1;
        for (int j = 0; j < 1023; j++)
        {
            if (map[i][j] != ' ')
            {
                fin = 0;
                break;
            }
        }
        if (!fin)
        {
            int lastCar = 0;
            for (int j = 0; j < 1023; j++)
            {
                if (map[i][j] != ' ')
                {
                    lastCar = j;
                }
            }
            map[i][lastCar + 1] = 0;
            printf("%s\n", map[i]);
        }
        free(map[i]);
    }
    free(map);
}