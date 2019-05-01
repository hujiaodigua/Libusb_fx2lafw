/*************************************************************************
	> File Name: Queue.c
	> Author: 
	> Mail: 
	> Created Time: Tue 30 Apr 2019 08:35:35 PM +08
 ************************************************************************/

#include"Queue.h"


QElemType d;
LinkQueue q;




int InitQueue(LinkQueue *Q)    // 构造一个空队列
{ 
    Q->front=Q->rear=(QueuePtr)malloc(sizeof(QNode));

    if(!Q->front)
        exit(OVERFLOW);

    Q->front->next=NULL;
    return OK;
}

int DestroyQueue(LinkQueue *Q) // 销毁队列
{
    while(Q->front)
    {
        Q->rear=Q->front->next;
        free(Q->front);
        Q->front=Q->rear;    
    }
    return OK;
}

int ClearQueue(LinkQueue *Q) // 清空队列
{
    QueuePtr p,q;
    Q->rear=Q->front;
    p=Q->front->next;
    Q->front->next=NULL;
    while(p)
    {
        q=p;
        p=p->next;
        free(q);    
    }
    return OK;
}

int QueueEmpty(LinkQueue Q)
{ 
    if(Q.front==Q.rear)
        return TRUE;         // 队列为空返回TRUE
    else
        return FALSE;        // 队列不为空返回FALES
}

int QueueLength(LinkQueue Q) // 求队列的长度
{ 
    int i=0;
    QueuePtr p;
    p=Q.front;
    while(Q.rear!=p)
    {
        i++;
        p=p->next;                    
    }
    return i;
}

int GetHead(LinkQueue Q,QElemType *e)  // 获得队头元素，若队列不空,则用e返回Q的队头元素,并返回OK,否则返回ERROR 
{ 
    QueuePtr p;

    if(Q.front==Q.rear)
        return ERROR;

    p=Q.front->next;
    *e=p->data;
    return OK;
}

int EnQueue(LinkQueue *Q,QElemType e)  // 插入元素e为Q的新的队尾元素
{ 
    QueuePtr s=(QueuePtr)malloc(sizeof(QNode));

    if(!s)                 // 存储分配失败
        exit(OVERFLOW);

    s->data=e;
    s->next=NULL;
    Q->rear->next=s;       // 把拥有元素e的新结点s赋值给原队尾结点的后继，见图中① */
    Q->rear=s;             // 把当前的s设置为队尾结点，rear指向s，见图中② */
    return OK;
}

int visit(QElemType data_inbuf)
{
    // if(out != NULL)
    // {
    // }

    int i;
    for(i = 0;i < buf_fill_len;i++)
    {
        printf("%x",data_inbuf.buf_fill[i]);
    }
    return OK;

}

int QueueTraverse(LinkQueue Q)   // 从队头到队尾依次对队列Q中每个元素输出
{
    QueuePtr p;
    p=Q.front->next;
    while(p)
    {
        visit(p->data);
        p=p->next;    
    }
    printf("\n");
    return OK;
}

int main()
{
    QElemType data_inbuf;    // 队里数据域的基本单位是data_inbuf

    int i;
    int k;


    k = InitQueue(&q);
    if(k)
    {
	printf("成功地构造了一个空队列!\n");
    }
    ClearQueue(&q);


    for(i = 0;i < buf_fill_len;i++)
    {
        data_inbuf.buf_fill[i] = 0xAA;
    }
    EnQueue(&q, data_inbuf);

    for(i = 0;i < buf_fill_len;i++)
    {
        data_inbuf.buf_fill[i] = 0x55;
    }
    EnQueue(&q, data_inbuf);


    QueueTraverse(q);

    ClearQueue(&q);
}
