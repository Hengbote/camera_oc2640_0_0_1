#include <rtthread.h>
#include <rtdevice.h>
#include "pin_config.h"
#include "OV2640.h"
#include "OV2640_config.h"
#include "i2c.h"

#define pictureBufferLength 1024*10
static uint32_t JpegBuffer[pictureBufferLength];

#define DBG_TAG "OV2640"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

extern DCMI_HandleTypeDef hdcmi;
OV2640_IDTypeDef OV2640ID;                  //设备id
extern Camera_Structure camera_device_t;    //摄像头设备
rt_thread_t camera_response_t;              //摄像头任务结构体
static struct rt_semaphore dcmi_sem;        //DCMI帧事件中断 回调函数信号量

/**
* @brief  读取摄像头的ID.
* @param  OV2640ID: 存储ID的结构体
* @retval None
*/
rt_err_t OV2640_ReadID(OV2640_IDTypeDef *OV2640ID)
{
    /*OV2640有两组寄存器，设置0xFF寄存器的值为0或为1时可选择使用不同组的寄存器*/
    write_reg_1data(OV2640_DSP_RA_DLMT, 0x01);

    /*读取寄存芯片ID*/
    read_reg_1data(OV2640_SENSOR_MIDH, &OV2640ID->Manufacturer_ID1);
    read_reg_1data(OV2640_SENSOR_MIDL, &OV2640ID->Manufacturer_ID2);
    read_reg_1data(OV2640_SENSOR_PIDH, &OV2640ID->PIDH);
    read_reg_1data(OV2640_SENSOR_PIDL, &OV2640ID->PIDL);

    if( OV2640ID->Manufacturer_ID1 != 0x7f  &&
        OV2640ID->Manufacturer_ID2 != 0xa2  &&
        OV2640ID->PIDH != 0x26              &&
        OV2640ID->PIDL != 0x42)
        return -RT_ERROR;

    return RT_EOK;
}

void OV2640_JPEGConfig(ImageFormat_TypeDef ImageFormat) //图片格式大小
{
  uint32_t i;

  for(i=0; i<(sizeof(OV2640_JPEG_INIT)/2); i++)
  {
      write_reg_1data(OV2640_JPEG_INIT[i][0], OV2640_JPEG_INIT[i][1]);
        //delay_ms(1);
  }


  for(i=0; i<(sizeof(OV2640_YUV422)/2); i++)
  {
      write_reg_1data(OV2640_YUV422[i][0], OV2640_YUV422[i][1]);
        //delay_ms(1);
  }

  write_reg_1data(0xff, 0x01);
  write_reg_1data(0x15, 0x00);


  for(i=0; i<(sizeof(OV2640_JPEG)/2); i++)
  {
      write_reg_1data(OV2640_JPEG[i][0], OV2640_JPEG[i][1]);
        //delay_ms(1);
  }

  //delay_ms(100);

  switch(ImageFormat)
  {
    case JPEG_160x120:
    {
      for(i=0; i<(sizeof(OV2640_160x120_JPEG)/2); i++)
      {
          write_reg_1data(OV2640_160x120_JPEG[i][0], OV2640_160x120_JPEG[i][1]);
        //delay_ms(1);
      }
      break;
    }
    case JPEG_176x144:
    {
      for(i=0; i<(sizeof(OV2640_176x144_JPEG)/2); i++)
      {
          write_reg_1data(OV2640_176x144_JPEG[i][0], OV2640_176x144_JPEG[i][1]);
      }
      break;
    }
    case JPEG_320x240:
    {
      for(i=0; i<(sizeof(OV2640_320x240_JPEG)/2); i++)
            {
          write_reg_1data(OV2640_320x240_JPEG[i][0], OV2640_320x240_JPEG[i][1]);
                //delay_ms(1);
            }
      break;
    }
    case JPEG_352x288:
    {
      for(i=0; i<(sizeof(OV2640_352x288_JPEG)/2); i++)
      {
          write_reg_1data(OV2640_352x288_JPEG[i][0], OV2640_352x288_JPEG[i][1]);
      }
      break;
    }
    case JPEG_800x600:
        {
            for(i=0; i<(sizeof(OV2640_800x600_JPEG)/2); i++)
            {
                write_reg_1data(OV2640_800x600_JPEG[i][0], OV2640_800x600_JPEG[i][1]);
            //delay_ms(1);
            }
            break;
        }

    default:
    {
      for(i=0; i<(sizeof(OV2640_160x120_JPEG)/2); i++)
      {
          write_reg_1data(OV2640_160x120_JPEG[i][0], OV2640_160x120_JPEG[i][1]);
      }
      break;
    }
  }
}

void OV2640_BrightnessConfig(rt_uint8_t Brightness)
{
    write_reg_1data(0xff, 0x00);
    write_reg_1data(0x7c, 0x00);
    write_reg_1data(0x7d, 0x04);
    write_reg_1data(0x7c, 0x09);
    write_reg_1data(0x7d, Brightness);
    write_reg_1data(0x7d, 0x00);
}

void SCCB_WriteRegs(const rt_uint8_t* pbuf)
{
    while(1)
    {
        if((*pbuf == 0) && (*(pbuf + 1) == 0))
        {
            break;
        }
        else
        {
            write_reg_1data((rt_uint8_t)*pbuf++, (rt_uint8_t)*pbuf++);
        }
    }
}

void OV2640_AutoExposure(rt_uint8_t level)
{
    switch(level)
    {
        case 0:
            SCCB_WriteRegs(OV2640_AUTOEXPOSURE_LEVEL0);
            break;
        case 1:
            SCCB_WriteRegs(OV2640_AUTOEXPOSURE_LEVEL1);
            break;
        case 2:
            SCCB_WriteRegs(OV2640_AUTOEXPOSURE_LEVEL2);
            break;
        case 3:
            SCCB_WriteRegs(OV2640_AUTOEXPOSURE_LEVEL3);
            break;
        case 4:
            SCCB_WriteRegs(OV2640_AUTOEXPOSURE_LEVEL4);
            break;
        default:
            SCCB_WriteRegs(OV2640_AUTOEXPOSURE_LEVEL0);
            break;
    }

}

/* DCMI接收到一桢数据后产生帧事件中断，调用此回调函数 */
void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)     //帧事件中断 回调函数
{
    rt_sem_release(&dcmi_sem);      //发送信号量
}

void OV2640_Reponse_Callback(void *parameter)
{
    MX_GPDMA1_Init();
    MX_DCMI_Init();
    camera_device_t.lock        = rt_mutex_create("mutex_camera", RT_IPC_FLAG_FIFO);                    //创建摄像头互斥锁
    if(camera_device_t.lock     == RT_NULL)
        LOG_E("Cannot create mutex for camera device on '%s'", CAMERA_I2C_BUS_NAME);
    camera_device_t.uart        = rt_device_find(CAMERA_UART_NAME);                                     //寻找摄像头串口输出设备
    if(camera_device_t.uart     == RT_NULL)
        LOG_E("Cannot find camera device on '%s'", CAMERA_UART_NAME);
    camera_device_t.i2c         = (struct rt_i2c_bus_device *)rt_device_find(CAMERA_I2C_BUS_NAME);      //寻找摄像头IIC总线设备
    if(camera_device_t.i2c      == RT_NULL)
        LOG_E("Cannot find camera device on '%s'", CAMERA_I2C_BUS_NAME);
    camera_device_t.dcmi        = &hdcmi;                                                               //获取DCMI设备结构体地址
    if(camera_device_t.dcmi     == RT_NULL)
        LOG_E("Cannot find camera device on DCMI");


    rt_device_open(camera_device_t.uart, RT_DEVICE_OFLAG_WRONLY);   //打开摄像头串口输出设备 只读
    rt_sem_init(&dcmi_sem, "dcmi_sem", 0, RT_IPC_FLAG_FIFO);        //初始化帧事件信号量 先进先出模式

    if(rt_mutex_take(camera_device_t.lock, RT_WAITING_FOREVER) != RT_EOK)       //上锁
        LOG_E("Failed to obtain the mutex\r\n");
    if(OV2640_ReadID(&OV2640ID) != RT_EOK)          //读取摄像头ID
        LOG_E("Failed read the camera ID\r\n");
    OV2640_JPEGConfig(JPEG_176x144);
    OV2640_BrightnessConfig(0x20);
    OV2640_AutoExposure(0);
    rt_mutex_release(camera_device_t.lock);                                     //解锁

    while(1)
    {
        __HAL_DCMI_ENABLE_IT(camera_device_t.dcmi, DCMI_IT_FRAME);  //使用帧中断
        memset((void *)JpegBuffer,0,pictureBufferLength * 4);       //把接收BUF清空
        HAL_DCMI_Start_DMA(camera_device_t.dcmi, DCMI_MODE_SNAPSHOT,(rt_uint32_t)JpegBuffer, pictureBufferLength);//启动拍照
        rt_thread_mdelay(50);
//        if(rt_sem_take(&dcmi_sem, RT_WAITING_FOREVER) == RT_EOK)
//            LOG_I("ID\r\n");
//        {
//            HAL_DCMI_Suspend(camera_device_t.dcmi);   //拍照完成，挂起DCMI
//            HAL_DCMI_Stop(camera_device_t.dcmi);      //拍照完成，停止DMA传输
//            int pictureLength =pictureBufferLength;
//            while(pictureLength > 0)        //循环计算出接收的JPEG的大小
//            {
//                if(JpegBuffer[pictureLength-1] != 0x00000000)
//                    break;
//                pictureLength--;
//            }
//            pictureLength *= 4;//buf是uint32_t，下面发送是uint8_t,所以长度要*4
//
//            if(rt_mutex_take(camera_device_t.lock, RT_WAITING_FOREVER) != RT_EOK)       //上锁
//                LOG_E("Failed to obtain the mutex\r\n");
//            rt_device_write(camera_device_t.uart, 0, (rt_uint8_t*)JpegBuffer, pictureLength);
////            if(mHuart != NULL)
////                HAL_UART_Transmit(mHuart, (uint8_t*)JpegBuffer, pictureLength, 100000);
//            rt_mutex_release(camera_device_t.lock);                                     //解锁

//        }
    }
}

void OV2640_Reponse(void)
{
    camera_response_t = rt_thread_create("camera_response_t", OV2640_Reponse_Callback, RT_NULL, 2048, 10, 10);
    if(camera_response_t!=RT_NULL)rt_thread_startup(camera_response_t);
    LOG_I("I2C_Reponse Init Success\r\n");
}
