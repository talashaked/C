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
*/

#include "Structs.h"
#include <stdlib.h>
# include <string.h>

double findNormBeforeRoot(Vector *vec);

int vectorCompare1By1(const void *a, const void *b)
{
    if (a == NULL || b == NULL)
    {
        return 0;
    }
    int min = 0;
    Vector *firstV = (Vector *) a;
    Vector *secondV = (Vector *) b;
    if (firstV->len >= secondV->len)
    {
        min = secondV->len;
    }
    else
    {
        min = firstV->len;
    }
    for (int i = 0; i < min; i++)
    {
        if (firstV->vector[i] > secondV->vector[i])
        {
            return 1;
        }
        else if (firstV->vector[i] < secondV->vector[i])
        {
            return -1;
        }
    }
    if (firstV->len != secondV->len)
    {
        if (min == firstV->len)
        {
            return -1;
        }
        else
        {
            return 1;
        }
    }
    return 0;
}

int copyIfNormIsLarger(const void *vector, void *maxVector)
{
    if (vector == NULL || maxVector == NULL)
    {
        return 0;
    }
    Vector *maxV = (Vector *) maxVector;
    const Vector *vec = (Vector *) vector;
    if (maxV->vector == NULL)
    {
        maxV->vector = (double *) malloc(vec->len * sizeof(double));
        for (int i = 0; i < vec->len; i++)
        {
            maxV->vector[i] = vec->vector[i];
        }
        maxV->len = vec->len;
        return 1;
    }
    double maxNorm = findNormBeforeRoot((Vector *) maxVector);
    double curNorm = findNormBeforeRoot((Vector *) vector);
    if (curNorm > maxNorm)
    {
        maxV->vector = (double *) realloc(maxV->vector, vec->len * sizeof(double));
        for (int i = 0; i < vec->len; i++)
        {
            maxV->vector[i] = vec->vector[i];
        }
        maxV->len = vec->len;
    }
    return 1;
}

/**
 * @brief find the norma of a vector before clacultaing root.
 * @param vec - a vector given to calculate on it
 * @return - the value of the norm of the vector
 */
double findNormBeforeRoot(Vector *vec)
{
    if (vec == NULL)
    {
        return 0;
    }
    double sum = 0;
    for (int i = 0; i < vec->len; i++)
    {
        sum += (vec->vector[i] * vec->vector[i]);
    }
    return sum;
}

Vector *findMaxNormVectorInTree(RBTree *tree)
{
    Vector *maxV = (Vector *) malloc(sizeof(Vector));
    maxV->vector = (double *) malloc(0);
    maxV->len = 0;
    forEachRBTree(tree, copyIfNormIsLarger, maxV);
    return maxV;
}

void freeVector(void *vector)
{
    Vector *vec = (Vector *) vector;
    free(vec->vector);
    free(vec);
}

int stringCompare(const void *a, const void *b)
{
    return strcmp((char *) a, (char *) b);
}

int concatenate(const void *word, void *pConcatenated)
{
    if (word == NULL || pConcatenated == NULL)
    {
        return 0;
    }
    strcat((char *) pConcatenated, (char *) word);
    strcat((char *) pConcatenated, "\n");
    return 1;
}

void freeString(void *s)
{
    char *str = (char *) s;
    free(str);
}