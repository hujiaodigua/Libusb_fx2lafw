/*************************************************************************
	> File Name: test.c
	> Author: 
	> Mail: 
	> Created Time: 2019年04月30日 星期二 05时32分27秒
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t mutex;
int global_counter = 0;

void *thread_fn(void *arg)
{
    srand(time(NULL));
    int i = 0;
 
    while (1) {
 
        // pthread_mutex_lock(&mutex);	// 上锁，但锁在main中被锁上了，所以除了上锁该线程进入阻塞态，直到开锁
        printf("hello %d ",global_counter);     // 而且锁也已经上了, 下一个循环运行到这里, 又执行上锁就又进入阻塞态了
        // sleep(rand() % 3);	/*模拟长时间操作共享资源，导致cpu易主，产生与时间有关的错误*/
        printf("world\n");
        // pthread_mutex_unlock(&mutex);
	pthread_mutex_lock(&mutex);
        // sleep(rand() % 3);
        i++;
        if(i == 5)
        {break;}
 
    }
    
    return NULL;
}

int main(void)
{
    int flag = 5;
    pthread_t tid;
    srand(time(NULL));
 
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_lock(&mutex);         // 锁初始化完成，立刻上锁
    pthread_create(&tid, NULL, thread_fn, NULL);

    // pthread_mutex_lock(&mutex);	// 上锁 -- 其实锁就是一个二值信号量
    int i = 5;
    for(i = 5;i > 0;i--)
    {
    	sleep(1);
    	printf("%d\n",i);
    	global_counter = 233;
    	pthread_mutex_unlock(&mutex); // 开锁
    }

    pthread_join(tid, NULL);
 
    pthread_mutex_destroy(&mutex);
 
    return 0;                           //main中的return可以将整个进程退出
}
