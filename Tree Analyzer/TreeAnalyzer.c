/**
* @file manageStudents.c
* @author Tal Shaked <tal.shaked3@mail.huji.ac.il>
* @version 1.0
* @date 27 Nov 2019
*
* @brief A software that creates a tree and does actions on it
* @section LICENSE
* This program is not a free software;
*
*
* Input : a file with a structure of a graph
* Process: building the input, and then collectting different required stats on the graph
* Output : printing to user
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

/**
 * @brief - a struct the consists of :
 * value - the number that represents the node
 * distance - relevant when measuring distance from other nodes in a graph
 * num of sons - the amount of sons the node has
 * father - the node directing to this node in a graph
 * nodeSons - an array of nodes that represents all the nodes that this nodes direct an edge to
 * in a graph
 * previous in path - which is the value of the node which we used to get to the current one,
 * when measuring distance
 */
typedef struct Node
{
    int value;
    int distance;
    int numOfSons;
    struct Node *father;
    struct Node **nodeSons;
    int previousInPath;
} Node;
/**
 * @brief the Tree struct, consists of:
 * value - the number of nodes in the tree
 * allNodesPtr - a pointer to all the nodes in the Tree
 */
typedef struct AllTree
{
    int value;
    Node *allNodesPtr;
} AllTree;

#define DELIMITER " "
#define NOSONS "-"
static int nodeU;
static int nodeV;

int checkValidOfLine(char line[], int maxNode);

int checkValidInput(int numOfArgs, char *const *argv);

int checkNodeIsValid(char *nodeValue, int numOfNodesInTree);

int checkCharIsNum(char num[1025]);

int initNode(int value, int numOfSons, AllTree *mainTree);

AllTree *initTree(int n);

AllTree *parseFile(char *file, AllTree *mainTree);

int addToNodeSon(int fatherIndex, char sonIndex[], int sonCount, AllTree *mainTree);

Node *findRoot(AllTree *mainTree);

void bfsTalsEdition(Node *root, AllTree *mainTree);

int findMaxDist(int flag, AllTree *mainTree);

int findMinDist(AllTree *mainTree);

int findDiameterLength(Node *root, AllTree *mainTree);

void freeTree(AllTree *mainTree);

int checkFile(const FILE *fileToProcess);

int main(int argc, char *argv[])
{
    int valid;
    AllTree *mainTree = NULL;
    if ((valid = checkValidInput(argc, argv)) == 1)
    {
        nodeU = (int) strtol(argv[2], NULL, 10);
        nodeV = (int) strtol(argv[3], NULL, 10);
        mainTree = parseFile(argv[1], mainTree);
    }
    else
    {
        if (valid == -1)
        {
            fprintf(stderr, "Usage: TreeAnalyzer <Graph File Path> <First Vertex> <Second "
                            "Vertex>\n");
        }
        else
        {
            fprintf(stderr, "Invalid input\n");
        }
        exit(EXIT_FAILURE);
    }
    Node *root = findRoot(mainTree);
    bfsTalsEdition(root, mainTree);
    int max = findMaxDist(0, mainTree);
    int min = findMinDist(mainTree);
    printf("Root Vertex: %d\n", root->value);
    printf("Vertices Count: %d\n", (mainTree->value));
    printf("Edges Count: %d\n", (mainTree->value - 1));
    printf("Length of Minimal Branch: %d\n", min);
    printf("Length of Maximal Branch: %d\n", max);
    int diameter = findDiameterLength(root, mainTree);
    printf("Diameter Length: %d\n", diameter);
    printf("Shortest Path Between %d and %d: ", nodeU, nodeV);
    bfsTalsEdition(&mainTree->allNodesPtr[nodeV], mainTree);
    int curNode = nodeU;
    while (mainTree->allNodesPtr[curNode].value != nodeV)
    {
        printf("%d ", curNode);
        curNode = mainTree->allNodesPtr[curNode].previousInPath;

    }
    printf("%d\n", nodeV);
    freeTree(mainTree);
    return 0;
}


/********************************************************************************
*********************************************************************************
*******************         validity checks            **************************
*********************************************************************************
********************************************************************************/

/**
 * @brief - this is the main functions that checks the validity of the input.
 * first - numof args is checked. then the filre - if openable, and if approved - its inside is
 * checked.
 * @param numOfArgs - the num of args given to the program at run
 * @param argv - a pointer to the args given by the user in the cmd while running the program
 * @return - 0 if input is invalid, -1 if input args doesn't fit the required, and 1 if the input
 * is valid
 */
int checkValidInput(int numOfArgs, char *const *argv)
{
    int isValid = 1;
    int numOfNOdesInTree = 0;

    if (numOfArgs == 4)
    {
        FILE *fileToProcess;
        char *fileName = argv[1];
        char *nodeUstr = argv[2];
        char *nodeVstr = argv[3];
        fileToProcess = fopen(fileName, "r");
        if (fileToProcess == NULL)
        {
            return 0;
        }
        numOfNOdesInTree = checkFile(fileToProcess);
        if (numOfNOdesInTree == 0)
        {
            isValid = 0;
        }
        fclose(fileToProcess);
        if ((checkNodeIsValid(nodeUstr, numOfNOdesInTree) == 0) ||
            (checkNodeIsValid(nodeVstr, numOfNOdesInTree) == 0))
        {
            isValid = 0;
        }
        return isValid;
    }

    else // input args are invalid
    {
        return -1;
    }
}

/**
 * @brief this function checks the file given by the user in the input
 * @param fileToProcess - a pointer to an open file
 * @param isValid - a pointer the the invalid int, kept in the function - checkValidInput
 * @param numOfNodesInTree - the number of nodes in the tree, a pointer to the number being
 * counted in the checkValidInput function
 */
int checkFile(const FILE *fileToProcess)
{
    int isV = 1;
    int numOfNodesInTree = 0;
    int countLines = 0;
    char line[1025] = {0};
    fgets(line, 1025, (FILE *) fileToProcess);
    sscanf(line, "%[^\r\n]", line);
    if (strcmp(line, "") == 0)
    {
        isV = 0;
    }
    else if (checkCharIsNum(line) == 0)
    {

        isV = 0;
    }
    else if ((numOfNodesInTree = (int) strtol(line, NULL, 10)) <= 0)
    {
        isV = 0;
    }
    else
    {
        countLines++;
        while (fgets(line, 1024, (FILE *) fileToProcess) != NULL)
        {
            sscanf(line, "%[^\r\n]", line);
            if (countLines - 1 == numOfNodesInTree)
            {
                if (fgets(line, 1024, (FILE *) fileToProcess) != NULL)
                {
                    isV = 0;
                }
                break;
            }
            else if (checkValidOfLine(line, numOfNodesInTree) != 1)
            {
                isV = 0;
                break;
            }
            countLines++;
        }
        if (countLines - 1 != numOfNodesInTree)
        {
            isV = 0;
        }
    }
    if (isV == 0)
    {
        return 0;
    }
    else
    {
        return numOfNodesInTree;
    }
}


/**
 * given a line in the file, this funciton checks that it is valid to parse as a legal line
 * @param line - an array of char presenting a line from the file
 * @return 0 if not valid, 1 if valid
 */
int checkValidOfLine(char line[], int maxNode)
{
    int countSons = 0;
    char copy[1025];
    strcpy(copy, line);
    if (strcmp(copy, NOSONS) == 0) //if the line has only '-' it is valid
    {
        return 1;
    }
    char *nodeIndex = strtok(copy, DELIMITER); //else - go over the line. check it only has
    // numbers, that are legit (legal numbers smaller than the number of nodes in the tree)
    if (nodeIndex == NULL)
    {//if line is empty
        return 0;
    }
    while (nodeIndex != NULL)
    {
        if (checkNodeIsValid(nodeIndex, maxNode) == 0)
        {
            return 0;
        }
        countSons++;
        nodeIndex = strtok(NULL, DELIMITER);
    }
    int *allSons = (int *) malloc(countSons * sizeof(int));
    if (allSons == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    //now the check is to see whether a node appears more than once in a line.
    int countChecked = 0;
    strcpy(copy, line);
    nodeIndex = strtok(copy, DELIMITER);
    while (nodeIndex != NULL)
    {
        int number = (int) strtol(nodeIndex, NULL, 10);
        for (int i = 0; i < countChecked; i++)
        {
            if (number == allSons[i])
            {
                free(allSons);
                return 0;
            }
        }
        allSons[countChecked] = number;
        countChecked++;
        nodeIndex = strtok(NULL, DELIMITER);
    }
    free(allSons);
    return 1;
}

/**
 * @brief - this function checks, given a number as a pointer to char, whether it is only a number
 * @param num
 * @return
 */
int checkCharIsNum(char num[1025])
{
    char temp[1025] = {0};
    sscanf(num, "%[0-9]", temp);
    if (strcmp(temp, num) != 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

/**
 * @brief - this function checks whether when given a node value, if it is legal in the given
 * tree - which means if this node was already in the line, and if the node value is a valid
 * number between 0 to n-1.
 * @param nodeValue - the node value to check on
 * @param numOfNodesInTree - the number of nodes in the tree
 * @return - 0 if invalid, 1 if valid
 */
int checkNodeIsValid(char *nodeValue, int numOfNodesInTree)
{
    int isValid = 1;
    int intNodeIndex;
    if (nodeValue == NULL)
    {
        return 0;
    }
    if (checkCharIsNum(nodeValue) == 0)
    {
        isValid = 0;
    }
    else
    {
        intNodeIndex = (int) strtol(nodeValue, NULL, 10);
        if ((intNodeIndex >= numOfNodesInTree) || (intNodeIndex < 0))
        {
            isValid = 0;
        }
    }
    return isValid;
}

/********************************************************************************
*********************************************************************************
*************        Parsing & Initializing Tree         ************************
*********************************************************************************
********************************************************************************/

/**
 * @brief in this function the assumption is that the input is valid(readably valid, not
 * neccessarily logically valid). this functions restarts the global main tree of the program and
 * all the nodes, according to the file given.
 * @param file - the file given by the user in the input
 */
AllTree *parseFile(char *file, AllTree *mainTree)
{
    int countLines = 0;
    int numOfNOdesInTree;
    char *sonNodeIndex;
    char line[1024];
    char analyzeLine[1024];
    int nodeSons = 0;
    int sonsCount = 0;
    FILE *fileToProcess;
    fileToProcess = fopen(file, "r");
    fgets(line, 1024, (FILE *) fileToProcess);
    numOfNOdesInTree = (int) strtol(line, NULL, 10);
    mainTree = initTree(numOfNOdesInTree);
    countLines++;
    while (fgets(line, 1024, (FILE *) fileToProcess) != NULL)
    {
        nodeSons = 0;
        sscanf(line, "%[^\r\n]", line);
        strcpy(analyzeLine, line);
        sonNodeIndex = strtok(analyzeLine, DELIMITER);
        if (strcmp(sonNodeIndex, NOSONS) != 0)
        {
            nodeSons = 0;
            while (sonNodeIndex != NULL)
            {
                nodeSons++;
                sonNodeIndex = strtok(NULL, DELIMITER);
            }
        }
        if (initNode(countLines - 1, nodeSons, mainTree) == 1)
        {
            if (nodeSons > 0)
            {
                strcpy(analyzeLine, line);
                sonNodeIndex = strtok(analyzeLine, DELIMITER);
                sonsCount = 0;
                while (sonNodeIndex != NULL)
                {
                    sonsCount++;
                    addToNodeSon(countLines - 1, sonNodeIndex, sonsCount - 1, mainTree);
                    sonNodeIndex = strtok(NULL, DELIMITER);
                }
            }
        }
        else
        {
            for (int i = 0; i <= countLines - 1; i++)
            {
                free(mainTree->allNodesPtr[i].nodeSons);
            }
            free(mainTree->allNodesPtr);
            free(mainTree);
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }

        countLines++;
    }
    fclose(fileToProcess);
    return mainTree;
}

/**
 * @brief initializing the global tree of the program
 * @param n - the number of nodes in the graph
 */
AllTree *initTree(int n)
{
    AllTree *mainTree = (AllTree *) malloc(sizeof(AllTree));
    if (mainTree == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    mainTree->allNodesPtr = (Node *) malloc(n * sizeof(Node));
    if (mainTree->allNodesPtr == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        free(mainTree);
        exit(EXIT_FAILURE);
    }
    mainTree->value = n;
    for (int i = 0; i < n; i++)
    {
        mainTree->allNodesPtr[i].father = NULL;
        mainTree->allNodesPtr[i].nodeSons = NULL;
        mainTree->allNodesPtr[i].numOfSons = 0;
        mainTree->allNodesPtr[i].previousInPath = -1;
        mainTree->allNodesPtr[i].value = i;
    }
    return mainTree;
}

/**
 * @brief initializing a node.
 * @param value -the value of the given node to initialize
 * @param numOfSons - the number of edges referred to from the current node given.
 */
int initNode(int value, int numOfSons, AllTree *mainTree)
{
    mainTree->allNodesPtr[value].numOfSons = numOfSons;
    if (numOfSons != 0)
    {
        mainTree->allNodesPtr[value].nodeSons = (Node **) malloc(numOfSons * sizeof(Node));
        if (mainTree->allNodesPtr[value].nodeSons == NULL)
        {
            return 0;
        }
    }
    return 1;
}

/**
 *  add to the given node's nodeSons pointer all the nodes that there is an edge referred to them
 *  from this node.
 * @return 1 if action successful, 0 otherwise
 */
int addToNodeSon(int fatherIndex, char sonIndex[], int sonCount, AllTree *mainTree)
{
    int intSonIndex;
    intSonIndex = (int) strtol(sonIndex, NULL, 10);
    mainTree->allNodesPtr[fatherIndex].nodeSons[sonCount] = &mainTree->allNodesPtr[intSonIndex];
    if (mainTree->allNodesPtr[intSonIndex].father == NULL)
    {
        mainTree->allNodesPtr[intSonIndex].father = &mainTree->allNodesPtr[fatherIndex];
        return 1;
    }
    else
    {
        return 0;
    }
}

/********************************************************************************
*********************************************************************************
**************         Functions On Tree            *****************************
*********************************************************************************
********************************************************************************/
/**
 * @brief finds the root of the global tree, based on the fact that the root is the only node
 * with no father(=no edge directed to him)
 * @return - the address of the root. if no nodes, or no more than one node is in the tree,
 * returns its address.
 */
Node *findRoot(AllTree *mainTree)
{
    for (int i = 0; i < mainTree->value; i++)
    {
        if (mainTree->allNodesPtr[i].father == NULL)
        {
            return &mainTree->allNodesPtr[i];
        }
    }
    return mainTree->allNodesPtr;
}

/**
 * @brief - the BFS algorithm matching to work on out current tree. given a relative root, and a
 * graph which is in this case global, it measures all the distances from this root to the nodes
 * connected to it.
 * @param root - a relative node which we want to measure distances from
 */
void bfsTalsEdition(Node *root, AllTree *mainTree)
{
    int curIndex;
    for (int i = 0; i < mainTree->value; i++)
    {
        mainTree->allNodesPtr[i].distance = -1;
    }
    Queue *q = allocQueue();
    root->distance = 0;
    enqueue(q, root->value);
    while (!queueIsEmpty(q))
    {
        curIndex = (int) dequeue(q);
        for (int i = 0; i < mainTree->allNodesPtr[curIndex].numOfSons; i++)
        {
            if (mainTree->allNodesPtr[curIndex].nodeSons[i]->distance == -1)
            {
                enqueue(q, mainTree->allNodesPtr[curIndex].nodeSons[i]->value);
                mainTree->allNodesPtr[curIndex].nodeSons[i]->distance =
                        mainTree->allNodesPtr[curIndex].distance + 1;
                mainTree->allNodesPtr[curIndex].nodeSons[i]->previousInPath =
                        mainTree->allNodesPtr[curIndex]
                                .value;
            }
        }
        if (mainTree->allNodesPtr[curIndex].father != NULL)
        {
            if (mainTree->allNodesPtr[curIndex].father->distance == -1)
            {
                enqueue(q, mainTree->allNodesPtr[curIndex].father->value);
                mainTree->allNodesPtr[curIndex].father->distance = mainTree->allNodesPtr[curIndex]
                                                                           .distance + 1;
                mainTree->allNodesPtr[curIndex].father->previousInPath =
                        mainTree->allNodesPtr[curIndex].value;
            }
        }
    }
    freeQueue(&q);
}

/**
 * @brief this function iterates over all the nodes in our tree, and returns the one having the
 * maximum distance. recommended to run after using BFS
 * @param flag - if 1 is given, the function returns the value of the node in the maximum
 * distance place. if 0 - the the distance value.
 * @return given above.
 */
int findMaxDist(int flag, AllTree *mainTree)
{
    int max = mainTree->allNodesPtr[0].distance;
    int tempIndex = mainTree->allNodesPtr[0].value;
    for (int i = 0; i < mainTree->value; i++)
    {
        if (mainTree->allNodesPtr[i].distance > max)
        {
            max = mainTree->allNodesPtr[i].distance;
            tempIndex = mainTree->allNodesPtr[i].value;
        }
    }
    if (flag == 0)
    {
        return max;
    }
    else
    {
        return tempIndex;
    }
}

/**
 * @brief in this function we iterate over all the distances of the nodes in the tree, and return
 * the distance of the lowest.
 * recommended to run after using BFS, relevant to return the minimum dist from a specific node.
 * @return - the minimum distance of an node in the tree from a node the last bfs was run on.
 */
int findMinDist(AllTree *mainTree)
{
    int min = mainTree->value;
    for (int i = 0; i < mainTree->value; i++)
    {
        if (mainTree->allNodesPtr[i].distance < min && mainTree->allNodesPtr[i].numOfSons == 0)
        {
            min = mainTree->allNodesPtr[i].distance;
        }
    }
    return min;
}

/**
 * @brief find the longest route in the tree. first we run a bfs from the root. then we run a bfs
 * on the furthest node from the root. the maximum route would be given by the furthest node from
 * the furthest node from the root.
 * @param root - the root of the tree
 * @return - the distance of the longest route in the tree
 */
int findDiameterLength(Node *root, AllTree *mainTree)
{
    bfsTalsEdition(root, mainTree);

    int max = findMaxDist(1, mainTree);
    bfsTalsEdition(&mainTree->allNodesPtr[max], mainTree);
    return findMaxDist(0, mainTree);

}

/********************************************************************************
*********************************************************************************
**************            Freeing Space             *****************************
*********************************************************************************
********************************************************************************/
/**
 * @brief free all the memory kept in the heap
 */
void freeTree(AllTree *mainTree)
{
    for (int i = 0; i < mainTree->value; i++)
    {
        free(mainTree->allNodesPtr[i].nodeSons);
    }
    free(mainTree->allNodesPtr);
    free(mainTree);
}

