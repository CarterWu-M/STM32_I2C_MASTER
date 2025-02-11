#include "API/Queue.h"
#include "string.h"

int Queue_init(Queue *pQ) {
	if (!pQ) {
		return -1;
	}
	pQ->front = 0;
	pQ->rear = -1;
	pQ->size = 0;
	return 0;
}

bool Queue_isEmpty(Queue *pQ) {
	if (!pQ) {
		return true;
	}
	return 0 == pQ->size;
}

bool Queue_isFull(Queue *pQ) {
	if (!pQ) {
		return true;
	}
	return QUEUE_SIZE == pQ->size;
}

int Queue_enqueue(Queue *pQ, uint8_t arrData[MAX_DATA_LEN], uint8_t dataLen) {
	if (Queue_isFull(pQ) || !arrData || !dataLen || MAX_DATA_LEN < dataLen) {
		return -1;
	}
	pQ->rear = (pQ->rear + 1) % QUEUE_SIZE;
	memcpy(pQ->arrData[pQ->rear], arrData, dataLen);
	pQ->arrDataLen[pQ->rear] = dataLen;
	pQ->size++;
	return 0;
}

int Queue_dequeue(Queue *pQ, uint8_t arrData[MAX_DATA_LEN], uint8_t *pDataLen) {
	if (Queue_isEmpty(pQ) || !arrData || !pDataLen) {
		return -1;
	}
	uint8_t dataLen = pQ->arrDataLen[pQ->front];
	memcpy(arrData, pQ->arrData[pQ->front], dataLen);
	*pDataLen = dataLen;
	pQ->front = (pQ->front + 1) % QUEUE_SIZE;
	pQ->size--;
	return 0;
}
