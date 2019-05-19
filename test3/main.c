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

#define NUMS 10

QElemType d;
LinkQueue q;

QElemType data_inbuf; 

int var_flag = 0;

char *filename;

struct libusb_transfer *img_transfer;
libusb_device_handle *dev_handle;

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
    // printf("ran send_samplerate\n");
    r = libusb_control_transfer(devhdl,LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_OUT, 0xb1, 0x0000, 0x0000, byte_cmd, sizeof(byte_cmd),100);
}




void LIBUSB_CALL fn_recv(struct libusb_transfer *transfer)
{
    // int ret;
    
    // printf("submit ret = %d\n", ret);
    if(var_flag <  NUMS)
    {  
        // printf("ran fn_recv\n");
        EnQueue(&q, data_inbuf);
        var_flag++;
        // printf("fn_recv var_flag = %d\n",var_flag);
        // ret = libusb_submit_transfer(img_transfer);
        libusb_submit_transfer(img_transfer);
    }
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
    static libusb_context *ctx = NULL;      // a libusb session
    int ret;                                // for return
    ssize_t cnt;                            // hold number of devices in list

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

    ret = libusb_claim_interface(dev_handle, 0);
    if(ret < 0)
    {
        perror("Cannot Claim Interface\n");
    }
    
    get_fw_version(dev_handle);

    printf("ran get_fw_version.\n");
    
    get_revid_version(dev_handle);
    printf("ran get_revid_version.\n");
 

    int count = NUMS;
    
    img_transfer = libusb_alloc_transfer(0);
    
    libusb_fill_bulk_transfer(img_transfer, dev_handle, 2 | LIBUSB_ENDPOINT_IN, data_inbuf.buf_fill,  buf_fill_len, fn_recv, NULL, 0);
    libusb_submit_transfer(img_transfer);
    
    send_samplerate(dev_handle);
  
    //sleep(5);
    while(TRUE)
    {
        if(var_flag <  NUMS)
        {
            send_samplerate(dev_handle);
        }
        libusb_handle_events(ctx);
        // libusb_handle_events_completed(ctx, NULL);        // 这个阻塞到底是什么意思，感觉很奇怪，是挂起当前进程吗
        // printf("running while\n");
        if(var_flag == NUMS)
        {
            printf("if varflag\n");
            libusb_release_interface(dev_handle,0);
            // libusb_close(dev_handle);
            libusb_exit(NULL);
            QueueTraverse(q);
            ClearQueue(&q);
            break;
        }
    }

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

}
