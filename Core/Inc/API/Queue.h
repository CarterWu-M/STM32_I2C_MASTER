
#ifndef SRC_QUEUE_H_
#define SRC_QUEUE_H_

#include "main.h"
#include "stdbool.h"

#define QUEUE_SIZE (5)
#define MAX_DATA_LEN (20)

typedef struct {
    uint8_t arrData[QUEUE_SIZE][MAX_DATA_LEN];
    uint8_t arrDataLen[QUEUE_SIZE];
    uint8_t front;
    int8_t rear;
    uint8_t size;
} Queue;

int Queue_init(Queue *pQ);
bool Queue_isEmpty(Queue *pQ);
bool Queue_isFull(Queue *pQ);
int Queue_enqueue(Queue *pQ, uint8_t arrData[MAX_DATA_LEN], uint8_t dataLen);
int Queue_dequeue(Queue *pQ, uint8_t arrData[MAX_DATA_LEN], uint8_t *pDataLen);

#endif /* SRC_QUEUE_H_ */
