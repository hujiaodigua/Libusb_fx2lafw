/*************************************************************************
	> File Name: main.c
	> Author: 
	> Mail: 
	> Created Time: 2019年04月18日 星期四 12时34分28秒
 ************************************************************************/
#include<libusb-1.0/libusb.h>
#include"Queue.h"

#define VID 0x0925
#define PID 0x3881

QElemType d;
LinkQueue q;

// static uint8_t buf_fill[buf_fill_len];
QElemType data_inbuf; 

int var_flag = 0;

// static libusb_device **devs;            // pointer to pointer of device
// static libusb_device_handle *dev_handle;// a device handle
// static libusb_context *ctx = NULL;      // a libusb session
// static struct libusb_transfer *img_transfer;

// struct version_info
// {
//     uint8_t major;
//     uint8_t minor;
// };

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
    unsigned char byte_cmd[3] = {0x40, 0x00, 0x01};
    r = libusb_control_transfer(devhdl,LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_OUT, 0xb1, 0x0000, 0x0000, byte_cmd, sizeof(byte_cmd),100);
}




static void LIBUSB_CALL fn_recv(struct libusb_transfer *transfer)
{
    if(var_flag < 3)
    {
        printf("ran fn_recv\n");

        //int i;
        printf("buffer:");
        // for(i = 0;i < buf_fill_len;i++)      // 每次进入回调都有buf_fill_len个字节的数据
        // {
        //     printf("%x", *transfer->buffer); // 存储下来,使用全局变量的多个buf,在回调里把他们的指针存下来,一个回调对应多次传输
        //     transfer->buffer++;
        // }
        printf("\n");

        EnQueue(&q, data_inbuf);

        // int k;
        // for(k = 0;k < 2;k++)
        // {
        //     printf("user_data %x\n", *(int *)transfer->user_data);
        //     transfer->user_data++;
        // }
        
        // if (transfer->status != LIBUSB_TRANSFER_COMPLETED) // LIBUSB_TRANSFER_COMPLETED = 0
        if(transfer->status == LIBUSB_TRANSFER_OVERFLOW)      // LIBUSB_TRANSFER_OVERFLOW = 6
        {
            fprintf(stderr, "transfer status %d\n", transfer->status);
            libusb_free_transfer(transfer);
            // exit(3);
        }
        printf("finish fn_recv\n"); 
        //  printf("%x\n",transfer->user_data);
        var_flag++;
    }
}




static int fill_bulk_transfer(libusb_device_handle *devhdl)
{
    /*int r;
    unsigned char buf[32];    // bulk_transfer长度不能超过int16 类型的65536
    int len;

    r = libusb_bulk_transfer(devhdl, 2 | LIBUSB_ENDPOINT_IN, buf, 32, &len, 100);
    // bulk_transfer就只能这么用,数据一次性的读到缓冲区里

    int i = 0;
    for(i = 0; i < sizeof(buf); i++)
    {
        if(i == 0)
        {
            printf("buf:");
        }

        printf("%x", buf[i]);

        if(i == sizeof(buf) - 1)
        {
            printf("\n");
        }
    }

    printf("len = %x\n",len);*/

    static struct libusb_transfer *img_transfer;
    // unsigned char buf_fill[32];
    unsigned char my_user_data[2] = {1, 2};
    // printf("my %d\n",*my_user_data);

    img_transfer = libusb_alloc_transfer(0);
    
    libusb_fill_bulk_transfer(img_transfer, devhdl, 2 | LIBUSB_ENDPOINT_IN,
                              data_inbuf.buf_fill, sizeof(data_inbuf.buf_fill), fn_recv, (void *)my_user_data, 0);
    // 长度参数是int类型，规定了buf的大小不能超过65536,
    // 而且实际上user_data的值是跟着buf在变的，这非常奇怪，也让人不解
    // 但是还有一种可能，user_data只是传递一个数据结构的指针过去，buf的数据就可以保存到这个数据结构中，
    // 当然这些操作都非常快
    // 所以fill_bulk_transfer就是提供一种便利，让使用者在回调函数里做一些快速的操作，比如数据在内存里的存储，而且sigrok好像就是这么写的

    libusb_submit_transfer(img_transfer);

    /*for(i = 0; i < sizeof(buf_fill); i++)
    {
        if(i == 0)
        {
            printf("buf_fill:");
        }

        printf("%x", buf_fill[i]);

        if(i == sizeof(buf_fill) - 1)
        {
            printf("\n");
        }
    }*/
}




int main()
{
    static libusb_device **devs;            // pointer to pointer of device
    static libusb_device_handle *dev_handle;// a device handle
    static libusb_context *ctx = NULL;      // a libusb session
    int ret;                                // for return
    ssize_t cnt;                            // hold number of devices in list

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

    libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_INFO);
    
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

    // libusb_has_capability(LIBUSB_CAP_SUPPORTS_DETACH_KERNEL_DRIVER);
    // libusb_kernel_driver_active(dev_handle, 0);

    ret = libusb_claim_interface(dev_handle, 0);
    if(ret < 0)
    {
        perror("Cannot Claim Interface\n");
    }

    
   
    
    // ret = libusb_control_transfer(dev_handle,LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_IN, 0xb0, 0x0000, 0x0000, vi, 2, 100);
    // vi按sigrok写，报段错误的原因应该在这里，这是一个接收参量
    get_fw_version(dev_handle);

    printf("ran get_fw_version.\n");

    // ret = libusb_control_transfer(dev_handle,LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_IN, 0xb2, 0x0000, 0x0000, revid, 1, 100);
    // revid同上
    get_revid_version(dev_handle);
    printf("ran get_revid_version.\n");

    // send_samplerate(dev_handle);

    int count = 3;
    while(count--)
    {
        fill_bulk_transfer(dev_handle);
        send_samplerate(dev_handle);
    }
    while(TRUE)
    {
        if(var_flag == 3)
        {
            QueueTraverse(q);
            ClearQueue(&q);
            break;
        }
    }

    printf("count = %d(3)\n",count);
    printf("var_flag = %d(3)\n",var_flag);

    if(ret < 0)
    {
        printf("ctl ret < 0\n");
    }
    else
    {
        printf("ctl ret !< 0, ret = %x\n",ret);
    }

    libusb_free_device_list(devs, 1);




}
