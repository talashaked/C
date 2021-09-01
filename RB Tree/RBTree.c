/**
* @file RBTree.c Struct.c
* @author Tal Shaked <tal.shaked3@mail.huji.ac.il>
* @version 1.0
* @date 12 Dec 2019
*
* @brief
* @section LICENSE
* This program is not a free software;
*
*
* Input : -
* Process: building an RBTree out of nodes, and then doing functions on them
* Output : as requested
*/
#include <stdio.h>
#include "RBTree.h"
#include <stdlib.h>

typedef enum Side
{
    Right,
    Left
} Side;

/**
 * creates a new node, returns NULL for failure in allocation
 * @param parent - the new nodes parent
 * @param left - the new nodes left son
 * @param right - the new nodes right son
 * @param data - the data given to the node
 * @return - the new node, null if failed to allocate memory
 */
Node *createNewNode(Node *parent, Node *left, Node *right, void *data);

/**
 * this function is called after inserting a new node to the tree. it check if the node, given as part of a tree, represents
 * a legal tree
 * assuming input of node is from a valid RBTRee. assuming node isn't NULL
 * @param newNode - the cur node the function is checking if his close family is part of a legal RB tree
 */
void checkForRotation(Node *newNode, RBTree *tree);

/**
 * rotate left the node given, as part of the tree
 * @param node the node you want to make a root of the sub-tree
 */
void rotateL(Node *node);

/**
 * rotate right the node given, as part of the tree
 * @param node the node you want to make a root of the sub-tree
 */
void rotateR(Node *node);

/**
 * after inserting a new node, if uncle is black fix tree accordingly as instructed
 * @param node - the node that was just inserted into the RBTree. therefore has a parent, and a grandparent
 */
void codeUncleBlack(Node *node, RBTree *tree);

/**
 * after inserting a new node, if uncle is red fix tree accordingly as instructed
 * @param node - the node that was just inserted into the RBTree. therefore has a parent, and a grandparent
 */
void codeUncleRed(Node *node, RBTree *tree);

/**
 *
 * @param side
 * @param node
 */
void rotation(Side side, Node *node, RBTree *tree);

/**
 * assuming assuming the node given isn't NULL. if it is NULL, the function will succeed but return NULL.
 * @param tree - assuming valid tree, if given NULL will crash.
 * @param temp -
 * @param data
 * @return - NULL if node is in tree, pointer to the father of the node to add to the tree if is isn't.
 */
Node *findPlace(RBTree *tree, void *data);

/**
 * go over all tree by order, from smallest to biggest, activate func on them
 * @param curNode - the node currently on
 * @param func - the func to activate on all of them
 * @param args - the args given to send to the func
 * @return - 1 for success, 0 otherwise
 */
int iterateTree(Node *curNode, forEachFunc func, void *args);

/**
 * iterate over the tree in order to free it all
 * @param node - the current node
 */
void iterateTreeFree(Node *node, FreeFunc func);

Node *createNewNode(Node *parent, Node *left, Node *right, void *data)
{
    Node *newNode = (Node *) malloc(sizeof(Node));
    if (newNode == NULL)
    {
        fprintf(stderr, "Allocation Failed!");
        return NULL;
    }
    newNode->left = left;
    newNode->right = right;
    newNode->parent = parent;
    newNode->data = data;
    newNode->color = RED;
    return newNode;
}


RBTree *newRBTree(CompareFunc compFunc, FreeFunc freeFunc)
{
    RBTree *newTree = (RBTree *) malloc(sizeof(RBTree));
    if (newTree == NULL)
    {
        fprintf(stderr, "Allocation Failed!");
        exit(EXIT_FAILURE);
    }
    newTree->compFunc = compFunc;
    newTree->freeFunc = freeFunc;
    newTree->root = NULL;
    newTree->size = 0;
    return newTree;
}

int addToRBTree(RBTree *tree, void *data)
{
    if (tree == NULL)
    {
        return 0;
    }
    Node *newNode = createNewNode(NULL, NULL, NULL, data);
    if (newNode == NULL) // allocation failed
    {
        return 0;
    }
    Node *temp = tree->root;
    if (temp == NULL)
    {
        newNode->color = BLACK;
        tree->root = newNode;
        tree->size++;
        return 1;
    }
    temp = findPlace(tree, data);
    if (temp == NULL)
    {
        free(newNode);
        return 0;
    }
    else
    {
        int comp = tree->compFunc(temp->data, data);
        if (comp < 0)
        {
            temp->right = newNode;
        }
        else
        {
            temp->left = newNode;
        }
        newNode->parent = temp;
        tree->size++;
        checkForRotation(newNode, tree);
        return 1;
    }
}

Node *findPlace(RBTree *tree, void *data)
{

    if (tree == NULL)
    {
        return NULL;
    }
    else if (tree->root == NULL)
    {
        return NULL;
    }
    Node *temp = tree->root;
    while (temp != NULL)
    {
        int comp = tree->compFunc(temp->data, data);
        if (comp == 0)
        {
            return NULL;
        }
        else
        {
            if (comp < 0)
            {
                if (temp->right != NULL)
                {
                    temp = temp->right;
                }
                else
                {
                    return temp;
                }
            }
            else
            {
                if (temp->left != NULL)
                {
                    temp = temp->left;
                }
                else
                {
                    return temp;
                }
            }
        }
    }
    return NULL;
}

void checkForRotation(Node *newNode, RBTree *tree)
{
    if (newNode == NULL)
    {
        return;
    }
    else if (newNode->parent == NULL)
    {
        newNode->color = BLACK;
        return;
    }
    else if (newNode->parent->color == BLACK)
    {
        return;
    }
    Node *uncleNode = newNode->parent->parent;
    if (uncleNode == NULL) // weird situation - uncle doesn't exist, father is red.. shouldnt happen
    {
        return;
    }
    if (uncleNode->left == newNode->parent)
    {
        uncleNode = uncleNode->right;
    }
    else
    {
        uncleNode = uncleNode->left;
    }
    //after defining uncleNode
    if (uncleNode == NULL) // empty means black
    {
        codeUncleBlack(newNode, tree);
    }
    else if (uncleNode->color == BLACK)
    {
        codeUncleBlack(newNode, tree);
    }
    else
    {
        codeUncleRed(newNode, tree);
    }
}


void codeUncleRed(Node *node, RBTree *tree)
{
    node->parent->parent->right->color = BLACK;
    node->parent->parent->left->color = BLACK;
    node->parent->parent->color = RED;
    checkForRotation(node->parent->parent, tree);
}

void codeUncleBlack(Node *node, RBTree *tree)
{
    if (node == NULL)
    {
        return;
    }
    else if (node->parent == NULL)
    {
        return;
    }
    Node *temp = node->parent;
    if (temp->left == node)
    {
        if (temp->parent->right == temp)
        {
            rotation(Right, node, tree);
            rotation(Left, node, tree);
        }
        else
        {
            rotation(Right, temp, tree);
        }
    }
    else if (temp->right == node)
    {
        if (temp->parent->left == temp)
        {
            rotation(Left, node, tree);
            rotation(Right, node, tree);
        }
        else
        {
            rotation(Left, temp, tree);
        }

    }
    else
    {
        return;
    }

}

void rotation(Side side, Node *node, RBTree *tree)
{
    if (node == NULL)
    {
        return;
    }
    Node *tempParent = node->parent;
    Node *afterRotationParent = tempParent->parent;
    if (side == Left)
    {
        rotateL(node);
    }
    else if (side == Right)
    {
        rotateR(node);
    }
    node->parent = afterRotationParent;
    if (node->parent != NULL)
    {
        if (node->parent->left == tempParent)
        {
            node->parent->left = node;
        }
        else if (node->parent->right == tempParent)
        {
            node->parent->right = node;
        }
    }
    else
    {
        tree->root = node;
    }

}

void rotateL(Node *node)
{
    Node *tempParent = node->parent;
    if (tempParent == NULL)
    {
        return;
    }
    Node *temp = node->left;
    node->left = tempParent;
    tempParent->parent = node;
    tempParent->right = temp;
    if (temp != NULL)
    {
        temp->parent = tempParent;
    }
    node->color = BLACK;
    tempParent->color = RED;
}

void rotateR(Node *node)
{
    Node *tempParent = node->parent;
    if (tempParent == NULL)
    {
        return;
    }
    Node *temp = node->right;
    node->right = tempParent;
    tempParent->parent = node;
    tempParent->left = temp;
    if (temp != NULL)
    {
        temp->parent = tempParent;
    }
    node->color = BLACK;
    tempParent->color = RED;
}

int containsRBTree(RBTree *tree, void *data)
{
    if (tree == NULL)
    {
        return 0;
    }
    else if (tree->root == NULL)
    {
        return 0;
    }
    Node *temp = findPlace(tree, data);
    if (temp == NULL)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int forEachRBTree(RBTree *tree, forEachFunc func, void *args)
{
    if (tree == NULL || tree->root == NULL)
    {
        return 0;
    }
    return iterateTree(tree->root, func, args);
}

int iterateTree(Node *curNode, forEachFunc func, void *args)
{
    int check = 0;
    if (curNode == NULL)
    {
        return 1;
    }
    else
    {
        check = iterateTree(curNode->left, func, args);
        if (check != 0)
        {
            check = func(curNode->data, args);
            if (check != 0)
            {
                check = iterateTree(curNode->right, func, args);
                if (check != 0)
                {
                    return 1;
                }
            }

        }
    }
    return 0;
}

void iterateTreeFree(Node *node, FreeFunc func)
{
    if (node == NULL)
    {
        return;
    }
    iterateTreeFree(node->left, func);
    iterateTreeFree(node->right, func);
    func(node->data);
    free(node);
}

void freeRBTree(RBTree *tree)
{
    if (tree == NULL)
    {
        return;
    }
    iterateTreeFree(tree->root, tree->freeFunc);
    free(tree);
}

