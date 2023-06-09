/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-03-25     RT-Thread    first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include "pin_config.h"
#include "main.h"
#if bare
    #include "i2c_Bare.h"
#else
    #include "i2c.h"
#include "OV2640.h"
#endif


#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

int main(void)
{
    int count = 1;

//    I2C_Reponse();
    OV2640_Reponse();
    while (count++)
    {
        rt_thread_mdelay(1000);
    }

    return RT_EOK;
}
