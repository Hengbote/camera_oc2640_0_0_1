#ifndef APPLICATIONS_OV2640_H_
#define APPLICATIONS_OV2640_H_

#include "stm32u5xx_hal.h"

/*摄像头寄存器地址*/
#define OV2640_DSP_RA_DLMT 0xFF         // DSP 地址
#define OV2640_SENSOR_MIDH 0x1C         //传感器 ID 高位
#define OV2640_SENSOR_MIDL 0x1D         //传感器 ID 低位
#define OV2640_SENSOR_PIDH 0x0A         //芯片的版本信息高位
#define OV2640_SENSOR_PIDL 0x0B         //芯片的版本信息低位

#define OV2640_BUS_ADDR 0x60            //总线读地址

//存储摄像头ID的结构体
typedef struct
{
    uint8_t Manufacturer_ID1;           //传感器 ID
    uint8_t Manufacturer_ID2;           //传感器 ID
    uint8_t PIDH;                       //芯片的版本信息
    uint8_t PIDL;                       //芯片的版本信息
} OV2640_IDTypeDef;

/* Image Sizes enumeration */

const static uint8_t OV2640_AUTOEXPOSURE_LEVEL0[]=
{
    0xFF,   0x01,   0xff,
    0x24,   0x20,   0xff,
    0x25,   0x18,   0xff,
    0x26,   0x60,   0xff,
    0x00,   0x00,   0x00
};
const static uint8_t OV2640_AUTOEXPOSURE_LEVEL1[]=
{
    0xFF,   0x01,   0xff,
    0x24,   0x34,   0xff,
    0x25,   0x1c,   0xff,
    0x26,   0x70,   0xff,
    0x00,   0x00,   0x00
};
const static uint8_t OV2640_AUTOEXPOSURE_LEVEL2[]=
{
    0xFF,   0x01,   0xff,
    0x24,   0x3e,   0xff,
    0x25,   0x38,   0xff,
    0x26,   0x81,   0xff,
    0x00,   0x00,   0x00
};
const static uint8_t OV2640_AUTOEXPOSURE_LEVEL3[]=
{
    0xFF,   0x01,   0xff,
    0x24,   0x48,   0xff,
    0x25,   0x40,   0xff,
    0x26,   0x81,   0xff,
    0x00,   0x00,   0x00
};
const static uint8_t OV2640_AUTOEXPOSURE_LEVEL4[]=
{
    0xFF,   0x01,   0xff,
    0x24,   0x58,   0xff,
    0x25,   0x50,   0xff,
    0x26,   0x92,   0xff,
    0x00,   0x00,   0x00
};

typedef enum
{
  BMP_QQVGA             =   0x00,       /* BMP 格式 QQVGA 160x120 大小 */
  BMP_QVGA              =   0x01,       /* BMP 格式 QVGA 320x240 大小 */
  JPEG_160x120          =   0x02,       /* JPEG 格式 160x120 大小 */
  JPEG_176x144          =   0x03,       /* JPEG 格式 176x144 大小 */
  JPEG_320x240          =   0x04,       /* JPEG 格式 320x240 大小 */
  JPEG_352x288          =   0x05,       /* JPEG 格式 352x288 大小 */
  JPEG_800x600          =   0x06
}ImageFormat_TypeDef;

rt_err_t OV2640_ReadID(OV2640_IDTypeDef *OV2640ID);
void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi);     //帧事件中断 回调函数
void OV2640_Reponse(void);  //任务调用

#endif /* APPLICATIONS_OV2640_H_ */
