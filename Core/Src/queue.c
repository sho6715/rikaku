/*
 * queue.c
 *
 *  Created on: Feb 11, 2021
 *      Author: shohe
 */

#include "queue.h"

void PrintQueue(queue_t* pQueue) {
	int i;
	for (i = 0; i <= SIZE - 1; i++) {
		printf("%d ", pQueue->data[i]);
	}
	printf("\n");
}

//キューの初期化
void InitQueue(queue_t* pQueue) {
	int i;
	//キューの中身を0埋め
	for (i = 0; i <= SIZE - 1; i++) {
		pQueue->data[i].x =  0 ;
		pQueue->data[i].y =  0 ;
		pQueue->data[i].step =  0 ;
	}
	//初期化
	pQueue->head = 0;
	pQueue->tail = 0;
	pQueue->flag = EMPTY;
//	PrintQueue(pQueue);
}


//enqueue関数
void EnQueue(queue_t* pQueue, stPOSITION value) {
//	Printf("enQ(%d)\n", value);
	//キューがFullの処理
	if (pQueue->flag == FULL) {
		printf("Full\n");
		return;
	}
	//キューがFullでないので、enqueue操作
	pQueue->data[pQueue->tail] = value;
	//リングバッファのため、tailが配列の終端だったら0にする
	if (pQueue->tail == SIZE - 1) {
		pQueue->tail = 0;
		//終端でなければ、tailをインクリメント
	}
	else {
		pQueue->tail++;
	}
	//フラグの更新
	if (pQueue->tail == pQueue->head) {
		pQueue->flag = FULL;
	}
	else {
		pQueue->flag = AVAILABLE;
	}
//	PrintQueue(pQueue);
}

//dequeue関数
stPOSITION DeQueue(queue_t* pQueue) {
//	printf("deQ\n");
	stPOSITION dequeue;
	//キューがEmptyの処理
	if (pQueue->flag == EMPTY) {
		printf("Empty\n");
		dequeue.x = 0;
		dequeue.y = 0;
		dequeue.step = 0;
		return  dequeue ;
	}
	dequeue = pQueue->data[pQueue->head];
	//キューがEmptyでなければ、dequeue操作
	pQueue->data[pQueue->head].x =  0 ;
	pQueue->data[pQueue->head].y =  0 ;
	pQueue->data[pQueue->head].step =  0 ;
	//リングバッファのため、headが配列の終端だったら0にする
	if (pQueue->head == SIZE - 1) {
		pQueue->head = 0;
		//終端でなければ、headをインクリメント
	}
	else {
		pQueue->head++;
	}
	//フラグの更新
	if (pQueue->tail == pQueue->head) {
		pQueue->flag = EMPTY;
	}
	else {
		pQueue->flag = AVAILABLE;
	}
//	PrintQueue(pQueue);

	return dequeue;
}
