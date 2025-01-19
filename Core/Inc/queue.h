/*
 * queue.h
 *
 *  Created on: Feb 11, 2021
 *      Author: shohe
 */

#ifndef INC_QUEUE_H_
#define INC_QUEUE_H_

#include "glob.h"

#define SIZE 64

typedef struct {
	uint8_t		x;
	uint8_t		y;
	uint16_t	step;
}stPOSITION;

enum status {
	EMPTY,
	AVAILABLE,
	FULL
};

typedef struct {
	stPOSITION data[SIZE];
	int head;
	int tail;
	int flag;
}queue_t;

//キューの中身をprint出力
void PrintQueue(queue_t* pQueue) ;
//キューの初期化
void InitQueue(queue_t* pQueue) ;
//enqueue関数
void EnQueue(queue_t* pQueue, stPOSITION value) ;
//dequeue関数
stPOSITION DeQueue(queue_t* pQueue);

#endif /* INC_QUEUE_H_ */
