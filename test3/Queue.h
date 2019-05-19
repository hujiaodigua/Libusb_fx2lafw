/*************************************************************************
	> File Name: Queue.h
	> Author: 
	> Mail: 
	> Created Time: Tue 30 Apr 2019 08:06:23 PM +08
 ************************************************************************/

#ifndef _QUEUE_H
#define _QUEUE_H

#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<math.h>
#include<libusb-1.0/libusb.h>

#define OK 1
#define ERROR 0

#define TRUE 1
#define FALSE 0

#define buf_fill_len 240128// Pi一次bulk传输只能传这么多字节,PC比这个大的多

static FILE *out;

//数据结构
typedef struct DataStruct
{
    uint8_t buf_fill[buf_fill_len];
}datastruct;

typedef datastruct QElemType;

typedef struct QNode    // 节点结构
{
    QElemType data;
    struct QNode *next;
}QNode, *QueuePtr;

typedef struct          // 队列的链表结构
{
    QueuePtr front, rear;
}LinkQueue;

int InitQueue(LinkQueue *Q);
int DestroyQueue(LinkQueue *Q);
int ClearQueue(LinkQueue *Q);
int QueueEmpty(LinkQueue Q);
int QueueLength(LinkQueue Q);
int GetHead(LinkQueue Q,QElemType *e);
int EnQueue(LinkQueue *Q,QElemType e);
int visit(QElemType data_inbuf);
int QueueTraverse(LinkQueue Q);



#endif

