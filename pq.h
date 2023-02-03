#ifndef SIMPLE_SHELL_PQ_H
#define SIMPLE_SHELL_PQ_H

#endif //SIMPLE_SHELL_PQ_H

typedef struct {
    char *value;
    int distance;
} PQItem;

typedef struct
{
    PQItem **queue;
    int size;
} PQ;

void insert(PQ *queue , PQItem *item);

PQItem *extractMin(PQ *queue);