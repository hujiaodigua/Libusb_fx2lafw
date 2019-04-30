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

#define OK 1
#define ERROR 0

#define TRUE 1
#define FALSE 0

#define buf_fill_len 65536*40

//数据结构
typedef struct DataStruct
{
    unsigned char buf_fill[buf_fill_len];
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


#endif

