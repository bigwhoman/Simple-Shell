#include "pq.h"
#include <stdio.h>

void swap(PQItem **first, PQItem **second) {
    PQItem *third = *first;
    *first = *second;
    *second = third;
}

int parent(int i)
{
 
    return (i - 1) / 2;
}

int leftChild(int i)
{
 
    return ((2 * i) + 1);
}

int rightChild(int i)
{
 
    return ((2 * i) + 2);
}

void shiftUp(PQ *pq, int i)
{
    while (i > 0 && pq->queue[parent(i)]->distance < pq->queue[i]->distance) {
        swap(&(pq->queue[parent(i)]), &(pq->queue[i]));
        i = parent(i);
    }
}

void shiftDown(PQ *pq, int i)
{
    int minIndex = i;
    int l = leftChild(i);
    if (l <= pq->size && pq->queue[l]->distance > pq->queue[minIndex]->distance) {
        minIndex = l;
    }
    int r = rightChild(i);
    if (r <= pq->size && pq->queue[r]->distance > pq->queue[minIndex]->distance) {
        minIndex = r;
    }
    if (i != minIndex) {
        swap(&(pq->queue[i]), &(pq->queue[minIndex]));
        shiftDown(pq, minIndex);
    }
}
 
void insert(PQ *pq , PQItem *p)
{
    pq->size = pq->size + 1;
    pq->queue[pq->size] = p;
    shiftUp(pq, pq->size);
}

PQItem *extractMin(PQ *pq)
{
    PQItem *result = pq->queue[0];
    pq->queue[0] = pq->queue[pq->size];
    pq->size = pq->size - 1;
    shiftDown(pq, 0);
    return result;
}