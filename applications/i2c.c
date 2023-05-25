#include <rtthread.h>
#include <rtdevice.h>
#include "pin_config.h"

#if !bare
#include "i2c.h"
#include "OV2640.h"

#define DBG_TAG "i2c"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>


Camera_Structure camera_device_t;     //摄像头设备
rt_thread_t i2c_response_t;      //摄像头任务结构体

void write_reg(rt_uint8_t reg, rt_uint8_t len, rt_uint8_t *buf)      //写 寄存器地址 数据长度 数据
{
    struct rt_i2c_msg msgs[2];

    //设备地址 -- 寄存器号
    msgs[0].addr = OV2640_BUS_ADDR>>1;
    msgs[0].flags = RT_I2C_WR | RT_I2C_NO_STOP;
//    msgs[0].flags = RT_I2C_WR;
    msgs[0].buf = &reg;
    msgs[0].len = 1;
    //打算往寄存器里写什么
    msgs[1].addr = OV2640_BUS_ADDR>>1;
    msgs[1].flags = RT_I2C_WR | RT_I2C_NO_START;
    msgs[1].buf = buf;
    msgs[1].len = len;

    rt_i2c_transfer(camera_device_t.i2c, msgs, 2);
}

void write_reg_1data(rt_uint8_t reg, rt_uint8_t buf)      //写 寄存器地址 数据
{
    write_reg(reg, 1, &buf);
}

void read_reg(rt_uint8_t reg, rt_uint8_t len, rt_uint8_t *buf)       //读 寄存器地址 数据长度 数据
{
    struct rt_i2c_msg msgs[2];

    msgs[0].addr = OV2640_BUS_ADDR>>1;
    msgs[0].flags = RT_I2C_WR;
    msgs[0].buf = &reg;
    msgs[0].len = 1;

    msgs[1].addr = OV2640_BUS_ADDR>>1;
    msgs[1].flags = RT_I2C_RD;
    msgs[1].buf = buf;
    msgs[1].len = len;

    rt_i2c_transfer(camera_device_t.i2c, msgs, 2);
}

void read_reg_1data(rt_uint8_t reg, rt_uint8_t *buf)       //写 寄存器地址 数据
{
    read_reg(reg, 1, buf);
}

void I2C_Reponse_Callback(void *parameter)
{
//    rt_uint8_t temp;
    OV2640_IDTypeDef OV2640ID;  //设备id

    /* 查找I2C总线设备，获取I2C总线设备句柄 */
    camera_device_t.i2c     = (struct rt_i2c_bus_device *)rt_device_find(CAMERA_I2C_BUS_NAME);      //寻找摄像头IIC总线设备
    if(camera_device_t.i2c  == RT_NULL)
        LOG_E("Cannot find camera device on '%s'", CAMERA_I2C_BUS_NAME);
    camera_device_t.lock    = rt_mutex_create("mutex_camera", RT_IPC_FLAG_FIFO);                    //创建摄像头互斥锁
    if(camera_device_t.lock == RT_NULL)
        LOG_E("Cannot create mutex for camera device on '%s'", CAMERA_I2C_BUS_NAME);

    while(1)
    {
        if(rt_mutex_take(camera_device_t.lock, RT_WAITING_FOREVER) != RT_EOK)       //上锁
            LOG_E("Failed to obtain the mutex\r\n");
//        write_reg();
//        read_reg(0x00, 1, &temp);
        OV2640_ReadID(&OV2640ID);
        rt_mutex_release(camera_device_t.lock);                                     //解锁

//        rt_kprintf("temp = %x\r\n", temp);
        rt_kprintf("Manufacturer_ID1 = %02x\r\n", OV2640ID.Manufacturer_ID1);
        rt_kprintf("Manufacturer_ID2 = %02x\r\n", OV2640ID.Manufacturer_ID2);
        rt_kprintf("PIDH = %02x\r\n", OV2640ID.PIDH);
        rt_kprintf("PIDL = %02x\r\n", OV2640ID.PIDL);
        rt_thread_mdelay(1000);
    }
}

//void I2C_Reponse(void)
//{
//    i2c_response_t = rt_thread_create("i2c_response_t", I2C_Reponse_Callback, RT_NULL, 2048, 10, 10);
//    if(i2c_response_t!=RT_NULL)rt_thread_startup(i2c_response_t);
//        LOG_I("I2C_Reponse Init Success\r\n");
//}

#endif
