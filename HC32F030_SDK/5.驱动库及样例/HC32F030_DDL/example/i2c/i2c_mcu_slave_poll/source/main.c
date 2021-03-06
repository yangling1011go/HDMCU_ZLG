/******************************************************************************
* Copyright (C) 2016, Huada Semiconductor Co.,Ltd All rights reserved.
*
* This software is owned and published by:
* Huada Semiconductor Co.,Ltd ("HDSC").
*
* BY DOWNLOADING, INSTALLING OR USING THIS SOFTWARE, YOU AGREE TO BE BOUND
* BY ALL THE TERMS AND CONDITIONS OF THIS AGREEMENT.
*
* This software contains source code for use with HDSC
* components. This software is licensed by HDSC to be adapted only
* for use in systems utilizing HDSC components. HDSC shall not be
* responsible for misuse or illegal use of this software for devices not
* supported herein. HDSC is providing this software "AS IS" and will
* not be responsible for issues arising from incorrect user implementation
* of the software.
*
* Disclaimer:
* HDSC MAKES NO WARRANTY, EXPRESS OR IMPLIED, ARISING BY LAW OR OTHERWISE,
* REGARDING THE SOFTWARE (INCLUDING ANY ACOOMPANYING WRITTEN MATERIALS),
* ITS PERFORMANCE OR SUITABILITY FOR YOUR INTENDED USE, INCLUDING,
* WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY, THE IMPLIED
* WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE OR USE, AND THE IMPLIED
* WARRANTY OF NONINFRINGEMENT.
* HDSC SHALL HAVE NO LIABILITY (WHETHER IN CONTRACT, WARRANTY, TORT,
* NEGLIGENCE OR OTHERWISE) FOR ANY DAMAGES WHATSOEVER (INCLUDING, WITHOUT
* LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION,
* LOSS OF BUSINESS INFORMATION, OR OTHER PECUNIARY LOSS) ARISING FROM USE OR
* INABILITY TO USE THE SOFTWARE, INCLUDING, WITHOUT LIMITATION, ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL OR CONSEQUENTIAL DAMAGES OR LOSS OF DATA,
* SAVINGS OR PROFITS,
* EVEN IF Disclaimer HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* YOU ASSUME ALL RESPONSIBILITIES FOR SELECTION OF THE SOFTWARE TO ACHIEVE YOUR
* INTENDED RESULTS, AND FOR THE INSTALLATION OF, USE OF, AND RESULTS OBTAINED
* FROM, THE SOFTWARE.
*
* This software may be replicated in part or whole for the licensed use,
* with the restriction that this Disclaimer and Copyright notice must be
* included with each copy of this software, whether used in part or whole,
* at all times.
*/
/******************************************************************************/
/** \file main.c
 **
 ** A detailed description is available at
 ** @link Sample Group Some description @endlink
 **
 **   - 2016-02-16  1.0  XYZ First version for Device Driver Library of Module.
 **
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "i2c.h"
#include "gpio.h"
/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/

/******************************************************************************
 * Local variable definitions ('static')                                      *
 ******************************************************************************/

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 ******************************************************************************
 ** \brief  Main function of project
 **
 ** \return uint32_t return value, if needed
 **
 ** This sample
 **
 ******************************************************************************/
uint8_t u8Recdata[10]={0x00};
uint32_t u32SendLen=0;
uint32_t u32RecvLen=0;
uint8_t u8State = 0;
en_result_t I2C_SlaveWriteData(en_i2c_channel_t enCh,uint8_t *pu8Data,uint32_t *u32Len)
{
    uint8_t u8i=0,u8State;
    while(1)
    {
        while(0 == I2C_GetIrq(enCh))
        {}
        u8State = I2C_GetState(enCh);
        switch(u8State)
        {
            case 0xA8:
            case 0xB0:
                I2C_WriteByte(enCh,pu8Data[u8i++]);
                break;
            case 0xB8: 
            case 0xC8:               
                I2C_WriteByte(enCh,pu8Data[u8i++]);            
                break;          
			case 0xC0://发送数据，接收非ACK
				break;
            default:               
                return ErrorInvalidParameter; 
        }
		I2C_ClearIrq(enCh);
		if(0xC0 == u8State)
        {
          return Ok;
        }
    } 
 }
en_result_t I2C_SlaveReadData(en_i2c_channel_t enCh,uint8_t *pu8Data,uint32_t *pu32Len)
{
    uint8_t u8i=0,u8State;
    while(1)
   {
        while(0 == I2C_GetIrq(enCh))
        {}
        u8State = I2C_GetState(enCh);
        switch(u8State)
        {
            case 0x60:
            case 0x68:
            case 0x70:
            case 0x78:
              break;
            case 0x80:
            case 0x90: 
			case 0x88://只读取一个字节情况:
			case 0x98:
                pu8Data[u8i++] = I2C_ReadByte(enCh);               
                break;
            case 0xA0:  //接收到停止条件    
                *pu32Len = u8i;
                break;	
            default:
                return ErrorInvalidParameter; 
        }
		I2C_ClearIrq(enCh);
        if(0xA0 == u8State)
        {
          return Ok;
        }
   }
}
int32_t main(void)
{
    stc_i2c_addr_t stcSlaveAddr;
    stc_gpio_config_t stcGpioCfg;
    stc_i2c_config_t stcI2cCfg;
    stc_sysctrl_clk_config_t stcCfg;
    
    DDL_ZERO_STRUCT(stcCfg);
    DDL_ZERO_STRUCT(stcI2cCfg);
    DDL_ZERO_STRUCT(stcGpioCfg);
    DDL_ZERO_STRUCT(stcSlaveAddr);
    
    Sysctrl_ClkSourceEnable(SysctrlClkRCL,TRUE);
    Sysctrl_SysClkSwitch(SysctrlClkRCL);
    Sysctrl_SetRCHTrim(SysctrlRchFreq24MHz);
	Sysctrl_SysClkSwitch(SysctrlClkRCH);
	Sysctrl_ClkSourceEnable(SysctrlClkRCL,FALSE);
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);
    Sysctrl_SetPeripheralGate(SysctrlPeripheralI2c0,TRUE);

    stcGpioCfg.enDir = GpioDirOut;
    stcGpioCfg.enOD = GpioOdEnable;
    stcGpioCfg.enPuPd = GpioPu;
    
    Gpio_Init(GpioPortB, GpioPin8,&stcGpioCfg);//端口初始化
    Gpio_Init(GpioPortB, GpioPin9,&stcGpioCfg);
    
    Gpio_SetAfMode(GpioPortB, GpioPin8,GpioAf1);//SCL
    Gpio_SetAfMode(GpioPortB, GpioPin9,GpioAf1);//SDA

    stcI2cCfg.enFunc = I2cBaud_En;
    stcI2cCfg.u8Tm = 0x02;//1M=(24000000/(8*(2+1))//波特率配置
    stcI2cCfg.pfnI2c0Cb = NULL;
    stcI2cCfg.bTouchNvic = FALSE;
    if(TRUE == stcI2cCfg.bTouchNvic)
	{
		EnableNvic(I2C0_IRQn,IrqLevel3,TRUE);
	} 
    I2C_DeInit(I2C0);
    stcSlaveAddr.Addr = 0x50;
    stcSlaveAddr.Gc = 0;
    
    I2C_Init(I2C0,&stcI2cCfg);//模块初始化
	
	I2C_WriteSlaveAddr(I2C0,&stcSlaveAddr);//写从机设备地址
	
    I2C_SetFunc(I2C0,I2cMode_En);//模块使能
    I2C_SetFunc(I2C0,I2cAck_En);//ACK打开
	I2C_SlaveReadData(I2C0,u8Recdata,&u32RecvLen);//等主机写数据
	I2C_SlaveWriteData(I2C0,u8Recdata,&u32SendLen);//等主机读数据
    while(1)
	{}
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


