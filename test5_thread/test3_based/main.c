/*************************************************************************
	> File Name: main.c
	> Author: 
	> Mail: 
	> Created Time: 2019年04月18日 星期四 12时34分28秒
 ************************************************************************/
#include<libusb-1.0/libusb.h>
#include<pthread.h>
#include"Queue.h"

#define VID 0x0925
#define PID 0x3881

#define NUMS 200
#define samplerate 12000000

#define SR_48MHz 48000000
#define SR_30MHz 30000000

QElemType d;
LinkQueue q;

QElemType data_inbuf; 

pthread_mutex_t mutex;
pthread_mutex_t mutex_a;

int var_flag = 0;

int EnQueue_flag = FALSE;

char *filename;

struct libusb_transfer *img_transfer;
libusb_device_handle *dev_handle;

int call_flag = FALSE; 
static libusb_context *ctx = NULL;      // a libusb session

// char buf_1[buf_fill_len];
// char buf_2[buf_fill_len];
// char buf_3[buf_fill_len];
/*********************
**获取固件版本********
**********************/
static int get_fw_version(libusb_device_handle *devhdl)
{
    int r;
    // struct version_info *vi;
    unsigned char byte_2_vi;
    r = libusb_control_transfer(devhdl,LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_IN, 0xb0, 0x0000, 0x0000, &byte_2_vi, 2, 100);
    printf("vi = %x\n",byte_2_vi);
}

/*********************
**获取revid版本********
**********************/
static int get_revid_version(libusb_device_handle *devhdl)
{
    int r;
    // unsigned char * revid;
    unsigned char byte_1_revid;
    r = libusb_control_transfer(devhdl,LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_IN, 0xb2, 0x0000, 0x0000, &byte_1_revid, 1, 100);
    printf("revid = %x\n",byte_1_revid);
}

/*********************
**发送采样率给固件********
**********************/
static int send_samplerate(libusb_device_handle *devhdl)
{
    int r;
    int delay;
    unsigned char flags, delay_h, delay_l;
    unsigned char byte_cmd[3];
    // unsigned char byte_cmd[3] = {0x40, 0x00, 0x01};
    // printf("ran send_samplerate\n");
    
    if(SR_48MHz % samplerate == 0)
    {
        flags = 0x40;
        delay = (int)(SR_48MHz / samplerate -1);
        if(delay > (6 * 256))
        {
            delay = (int)(0);
        }
    }

    if(delay == 0 && (SR_30MHz % samplerate == 0))
    {
        flags = 0x00;
        delay = (int)(SR_30MHz / samplerate - 1);
    }

    delay_h = (delay >> 8) & 0xff;
    delay_l = delay & 0xff;

    *byte_cmd = flags, *(byte_cmd+1) = delay_h, *(byte_cmd+2) = delay_l;

    r = libusb_control_transfer(devhdl,LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_OUT, 0xb1, 0x0000, 0x0000, byte_cmd, sizeof(byte_cmd),0);
}

void *send_samplerate_thread(void *arg)
{
    // while(TRUE)
    // {

        pthread_mutex_lock(&mutex);
        printf("samplerate thread\n");
        printf("......\n");
        // send_samplerate(dev_handle);
        // libusb_submit_transfer(img_transfer);

        // libusb_handle_events_completed(ctx, NULL);
        // libusb_submit_transfer(img_transfer);
        // libusb_submit_transfer(img_transfer);
        // libusb_submit_transfer(img_transfer);
        // libusb_submit_transfer(img_transfer);
        // libusb_submit_transfer(img_transfer);
        // libusb_submit_transfer(img_transfer);
        // libusb_submit_transfer(img_transfer);
        // libusb_submit_transfer(img_transfer);
        // libusb_submit_transfer(img_transfer);
        // sleep(1);
        // break;
        // send_samplerate(dev_handle);
        // send_samplerate(dev_handle);
        // send_samplerate(dev_handle);
        send_samplerate(dev_handle);
        libusb_handle_events(ctx);        // 如果不把这个函数放到发送采样率后面去阻塞
                                          // 提交的bulk_transfer的call的话,发送采样率用的
                                          // control_transfer的call就会影响到bulk_transfer的
                                          // 连续传输,就会导致丢失一些数据
        // pthread_mutex_lock(&mutex);
        // if(var_flag ==  NUMS)
        // {break;}
    // }
    return NULL;
}

void *SaveData_thread(void *arg)
{
    /*while(TRUE)
    {
        if(EnQueue_flag == TRUE)    
        {
            EnQueue_flag = FALSE;
            EnQueue(&q, data_inbuf);
        }
        if(var_flag == NUMS)
        {break;}
    }*/
    pthread_mutex_lock(&mutex_a);
    printf("finished\n");
}

void LIBUSB_CALL fn_recv(struct libusb_transfer *transfer)
{
    // int ret;
    // call_flag = TRUE;
    // printf("submit ret = %d\n", ret);
    //按理第一次
    
    usleep(80000);        // 我觉得这里必须要加延迟是因为,多次传输提交上去之后
                          // 采样率发送成功,数据连续读取后,所有的数据会先放到操作系统的buffer里,
                          // 自然就会一次又一次从操作系的buffer
                          // (这个buffer也许是队列结构的)进入bulk_transfer的buf里,这就需要时间
                          // 不然这次数据还没进到buf里,下次回调又进来了

    if(var_flag <  NUMS)
    {
        printf("call if\n");
        // printf("ran fn_recv\n");
        EnQueue(&q, data_inbuf);
        // EnQueue_flag = TRUE;
        // sleep(1);
        var_flag++;
        // printf("fn_recv var_flag = %d\n",var_flag);
        // ret = libusb_submit_transfer(img_transfer);
        // libusb_submit_transfer(img_transfer);
        // send_samplerate(dev_handle);
        // pthread_mutex_unlock(&mutex); // 开锁
    }
    // call_flag = FALSE;
}




static int fill_bulk_transfer(libusb_device_handle *devhdl)
{
    unsigned char my_user_data[2] = {1, 2};
    // printf("my %d\n",*my_user_data);
    printf("ran fill_bulk_transfer\n");

    img_transfer = libusb_alloc_transfer(0);
    
    libusb_fill_bulk_transfer(img_transfer, devhdl, 2 | LIBUSB_ENDPOINT_IN,
                              data_inbuf.buf_fill, sizeof(data_inbuf.buf_fill), fn_recv, (void *)my_user_data, 0);
    libusb_submit_transfer(img_transfer);
}




int main(int argc, char *argv[])
{
    static libusb_device **devs;            // pointer to pointer of device
    // static libusb_device_handle *dev_handle;// a device handle
    // static libusb_context *ctx = NULL;      // a libusb session
    int ret;                                // for return
    ssize_t cnt;                            // hold number of devices in list
    // struct timeval tv;
    pthread_t tid;
    pthread_t tid_SaveData;
    
    if(argc == 2)
    {
        filename = argv[1];
    }

    ret = InitQueue(&q);
    if(ret)
    {
	printf("成功地构造了一个空队列!\n");
    }
    ClearQueue(&q);

    ret = libusb_init(&ctx);                // initialize the library of the session
    if (ret < 0)
    {
        perror("Init error\n");
        return 1;
    }

    // libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_INFO);
    
    cnt = libusb_get_device_list(ctx,&devs);
    if (cnt < 0)
    {
        perror("Get Device error\n");
        return 1;
    }

    printf("%d Devices in list.\n",cnt);

    dev_handle = libusb_open_device_with_vid_pid(ctx, VID, PID);

    // printf("dev_handle = %x\n", dev_handle);

    if(dev_handle == NULL)
    {
        perror("Cannot open device\n");
    }
    else
    {
        printf("Device opened\n");
    }

    ret = libusb_claim_interface(dev_handle, 0);
    if(ret < 0)
    {
        perror("Cannot Claim Interface\n");
    }
    
    get_fw_version(dev_handle);

    printf("ran get_fw_version.\n");
    
    get_revid_version(dev_handle);
    printf("ran get_revid_version.\n");
 
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_lock(&mutex);         // 锁初始化完成，立刻上锁
    
    pthread_mutex_init(&mutex_a, NULL);
    pthread_mutex_lock(&mutex_a);

    pthread_create(&tid, NULL, send_samplerate_thread, NULL);
    // pthread_create(&tid_SaveData, NULL, SaveData_thread, NULL);
    int count = NUMS;
   
    while(count--)
    {
        // usleep(10);
        img_transfer = libusb_alloc_transfer(0);  
        libusb_fill_bulk_transfer(img_transfer, dev_handle, 2 |LIBUSB_ENDPOINT_IN, data_inbuf.buf_fill,  buf_fill_len, fn_recv, NULL, 0);
        ret = libusb_submit_transfer(img_transfer);
        if(ret != 0 )
        {
            printf("sub ret = %d,count = %d\n", ret, count);
        }
        usleep(10);
    }
    pthread_mutex_unlock(&mutex);
    // send_samplerate(dev_handle);
    pthread_join(tid, NULL);
    // pthread_join(tid_SaveData, NULL);
    /*pthread_mutex_init(&mutex, NULL);
    pthread_mutex_lock(&mutex);         // 锁初始化完成，立刻上锁
    
    pthread_create(&tid, NULL, send_samplerate_thread, NULL);
    // pthread_create(&tid_SaveData, NULL, SaveData_thread, NULL);
    pthread_join(tid, NULL);
    // pthread_join(tid_SaveData, NULL);

    int length = 0;*/
   
    /*// 两个sbumission和call,一个采样率，再一个submission 就能在call里接收到数据
    while(count--)
    {
        libusb_bulk_transfer(dev_handle, 2 |LIBUSB_ENDPOINT_IN, data_inbuf.buf_fill, buf_fill_len, &length, 1);
        libusb_bulk_transfer(dev_handle, 2 |LIBUSB_ENDPOINT_IN, data_inbuf.buf_fill, buf_fill_len, &length, 1);

        send_samplerate(dev_handle);
        libusb_bulk_transfer(dev_handle, 2 |LIBUSB_ENDPOINT_IN, data_inbuf.buf_fill, buf_fill_len, &length, 1);
        EnQueue(&q, data_inbuf);
    }*/

    /*tv.tv_sec =0;
    tv.tv_usec = 100;
    //sleep(5);*/
    /*while(TRUE)
    {
        if(call_flag == FALSE)
        {
            printf("while if call\n");
            if(var_flag <  NUMS)
            {
                send_samplerate(dev_handle);                    // 这个指令就像和fill_bulk绑定起来了一样，每次读取数据都得发这个指令，sigrok可不是这样的
                                                                // 事实是这里的send_samplerate发了很多出去
                sleep(1);                                   // 所以这里的大循环需要被挂起，不让大循环继续运行下去,等待callback运行完毕再继续执行
                // libusb_handle_events_locked(ctx, &tv); 
            }
            // libusb_handle_events(ctx);                       // 我突然觉得用阻塞的方法好像有问题，应该用libusb_handle_events_timeout,丢点与此无关
            // libusb_handle_events_timeout(ctx, &tv);
            // libusb_handle_events_completed(ctx, NULL);       // 这个阻塞到底是什么意思，感觉很奇怪，是挂起当前进程吗
            // printf("running while\n");
            if(var_flag == NUMS)
            {
                printf("if varflag\n");
                libusb_release_interface(dev_handle,0);
                // libusb_close(dev_handle);
                libusb_exit(NULL);
                break;
            }

        }
    }*/
    QueueTraverse(q);
    ClearQueue(&q);
    printf("count = %d(%d)\n",count,NUMS);
    printf("var_flag = %d(%d)\n",var_flag,NUMS);

    if(ret < 0)
    {
        printf("ctl ret < 0\n");
    }
    else
    {
        printf("ctl ret !< 0, ret = %x\n",ret);
    }

    libusb_free_device_list(devs, 1);

    libusb_release_interface(dev_handle, 0);
    libusb_close(dev_handle);
    libusb_exit(ctx);

}
