/*************************************************************************
	> File Name: main.c
	> Author: 
	> Mail: 
	> Created Time: 2019年04月18日 星期四 12时34分28秒
 ************************************************************************/

#include<unistd.h>
#include<stdio.h>
#include<libusb-1.0/libusb.h>

#define VID 0x0925
#define PID 0x3881

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
   printf("ran fn_recv"); 
   //  printf("%x\n",transfer->user_data);
}

static int bulk_transfer(libusb_device_handle *devhdl)
{
    int r;
    // struct libusb_transfer *transfer;
    unsigned char buf[32];
    // int recv[512];
    int len;

    // transfer = libusb_alloc_transfer(0);
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

    printf("recv = %x\n",len);

    struct libusb_transfer *transfer;
    // void * sdi;

    transfer = libusb_alloc_transfer(0);
    libusb_fill_bulk_transfer(transfer, devhdl, 2 | LIBUSB_ENDPOINT_IN, buf, 32, fn_recv,NULL , 100);
}


int main()
{
    libusb_device **devs;                // pointer to pointer of device
    libusb_device_handle *dev_handle;    // a device handle
    libusb_context *ctx = NULL;          // a libusb session
    int ret;                               // for return
    ssize_t cnt;                         // hold number of devices in list


    ret = libusb_init(&ctx);               // initialize the library of the session
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

    send_samplerate(dev_handle);

    bulk_transfer(dev_handle);

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
