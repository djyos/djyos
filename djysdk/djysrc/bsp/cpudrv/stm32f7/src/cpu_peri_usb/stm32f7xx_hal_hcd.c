/**
  ******************************************************************************
  * @file    stm32f7xx_hal_hcd.c
  * @author  MCD Application Team
  * @version V1.1.2
  * @date    23-September-2016 
  * @brief   HCD HAL module driver.
  *          This file provides firmware functions to manage the following 
  *          functionalities of the USB Peripheral Controller:
  *           + Initialization and de-initialization functions
  *           + IO operation functions
  *           + Peripheral Control functions 
  *           + Peripheral State functions
  *         
  @verbatim
  ==============================================================================
                    ##### How to use this driver #####
  ==============================================================================
  [..]
    (#)Declare a HCD_HandleTypeDef handle structure, for example:
       HCD_HandleTypeDef  hhcd;
        
    (#)Fill parameters of Init structure in HCD handle
  
    (#)Call HAL_HCD_Init() API to initialize the HCD peripheral (Core, Host core, ...) 

    (#)Initialize the HCD low level resources through the HAL_HCD_MspInit() API:
        (##) Enable the HCD/USB Low Level interface clock using the following macros
             (+++) __HAL_RCC_USB_OTG_FS_CLK_ENABLE();
             (+++) __HAL_RCC_USB_OTG_HS_CLK_ENABLE(); (For High Speed Mode)
             (+++) __HAL_RCC_USB_OTG_HS_ULPI_CLK_ENABLE(); (For High Speed Mode)
           
        (##) Initialize the related GPIO clocks
        (##) Configure HCD pin-out
        (##) Configure HCD NVIC interrupt
    
    (#)Associate the Upper USB Host stack to the HAL HCD Driver:
        (##) hhcd.pData = phost;

    (#)Enable HCD transmission and reception:
        (##) HAL_HCD_Start();

  @endverbatim
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"

/** @addtogroup STM32F7xx_HAL_Driver
  * @{
  */

/** @defgroup HCD HCD 
  * @brief HCD HAL module driver
  * @{
  */

#ifdef HAL_HCD_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function ----------------------------------------------------------*/
/** @defgroup HCD_Private_Functions HCD Private Functions
  * @{
  */
static void HCD_HC_IN_IRQHandler(HCD_HandleTypeDef *hhcd, uint8_t chnum);
static void HCD_HC_OUT_IRQHandler(HCD_HandleTypeDef *hhcd, uint8_t chnum); 
static void HCD_RXQLVL_IRQHandler(HCD_HandleTypeDef *hhcd);
static void HCD_Port_IRQHandler(HCD_HandleTypeDef *hhcd);
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup HCD_Exported_Functions HCD Exported Functions
  * @{
  */

/** @defgroup HCD_Exported_Functions_Group1 Initialization and de-initialization functions 
 *  @brief    Initialization and Configuration functions 
 *
@verbatim    
 ===============================================================================
          ##### Initialization and de-initialization functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
 
@endverbatim
  * @{
  */

/**
  * @brief  Initialize the host driver.
  * @param  hhcd: HCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HCD_Init(HCD_HandleTypeDef *hhcd)
{ 
  /* Check the HCD handle allocation */
  if(hhcd == NULL)
  {
    return HAL_ERROR;
  }
  
  /* Check the parameters */
  assert_param(IS_HCD_ALL_INSTANCE(hhcd->Instance));

  hhcd->State = HAL_HCD_STATE_BUSY;
  
  /* Init the low level hardware : GPIO, CLOCK, NVIC... */
  HAL_HCD_MspInit(hhcd); // 配置IO,使能时钟

  /* Disable the Interrupts */
 __HAL_HCD_DISABLE(hhcd);
 
 /*Init the Core (common init.) */
 USB_CoreInit(hhcd->Instance, hhcd->Init); //
 
 /* Force Host Mode*/
 USB_SetCurrentMode(hhcd->Instance, USB_OTG_HOST_MODE); // 设置为HOST模式
 
 /* Init Host */
 USB_HostInit(hhcd->Instance, hhcd->Init); // USB主模式初始化
 
 hhcd->State= HAL_HCD_STATE_READY; //
 
 return HAL_OK;
}

/**
  * @brief  Initialize a host channel.
  * @param  hhcd: HCD handle
  * @param  ch_num: Channel number.
  *         This parameter can be a value from 1 to 15
  * @param  epnum: Endpoint number.
  *          This parameter can be a value from 1 to 15
  * @param  dev_address : Current device address
  *          This parameter can be a value from 0 to 255
  * @param  speed: Current device speed.
  *          This parameter can be one of these values:
  *            HCD_SPEED_HIGH: High speed mode,
  *            HCD_SPEED_FULL: Full speed mode,
  *            HCD_SPEED_LOW: Low speed mode
  * @param  ep_type: Endpoint Type.
  *          This parameter can be one of these values:
  *            EP_TYPE_CTRL: Control type,
  *            EP_TYPE_ISOC: Isochronous type,
  *            EP_TYPE_BULK: Bulk type,
  *            EP_TYPE_INTR: Interrupt type
  * @param  mps: Max Packet Size.
  *          This parameter can be a value from 0 to32K
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HCD_HC_Init(HCD_HandleTypeDef *hhcd,  
                                  uint8_t ch_num,
                                  uint8_t epnum,
                                  uint8_t dev_address,
                                  uint8_t speed,
                                  uint8_t ep_type,
                                  uint16_t mps)
{
  HAL_StatusTypeDef status = HAL_OK;
  
  __HAL_LOCK(hhcd); 
  
  hhcd->hc[ch_num].dev_addr = dev_address;
  hhcd->hc[ch_num].max_packet = mps;
  hhcd->hc[ch_num].ch_num = ch_num;
  hhcd->hc[ch_num].ep_type = ep_type;
  hhcd->hc[ch_num].ep_num = epnum & 0x7F;
  hhcd->hc[ch_num].ep_is_in = ((epnum & 0x80) == 0x80);
  hhcd->hc[ch_num].speed = speed;

  status =  USB_HC_Init(hhcd->Instance, 
                        ch_num,
                        epnum,
                        dev_address,
                        speed,
                        ep_type,
                        mps);
  __HAL_UNLOCK(hhcd); 
  
  return status;
}

/**
  * @brief  Halt a host channel.
  * @param  hhcd: HCD handle
  * @param  ch_num: Channel number.
  *         This parameter can be a value from 1 to 15
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HCD_HC_Halt(HCD_HandleTypeDef *hhcd, uint8_t ch_num)
{
  HAL_StatusTypeDef status = HAL_OK;
  
  __HAL_LOCK(hhcd);   
  USB_HC_Halt(hhcd->Instance, ch_num);   
  __HAL_UNLOCK(hhcd);
  
  return status;
}

/**
  * @brief  DeInitialize the host driver.
  * @param  hhcd: HCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HCD_DeInit(HCD_HandleTypeDef *hhcd)
{
  /* Check the HCD handle allocation */
  if(hhcd == NULL)
  {
    return HAL_ERROR;
  }
  
  hhcd->State = HAL_HCD_STATE_BUSY;
  
  /* DeInit the low level hardware */
  HAL_HCD_MspDeInit(hhcd);
  
   __HAL_HCD_DISABLE(hhcd);
  
  hhcd->State = HAL_HCD_STATE_RESET; 
  
  return HAL_OK;
}

/**
  * @brief  Initialize the HCD MSP.
  * @param  hhcd: HCD handle
  * @retval None
  */
#if 0
__weak void  HAL_HCD_MspInit(HCD_HandleTypeDef *hhcd)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hhcd);
  
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_HCD_MspInit could be implemented in the user file
   */
}
#endif
/**
  * @brief  DeInitialize the HCD MSP.
  * @param  hhcd: HCD handle
  * @retval None
  */
__weak void  HAL_HCD_MspDeInit(HCD_HandleTypeDef *hhcd)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hhcd);
  
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_HCD_MspDeInit could be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup HCD_Exported_Functions_Group2 Input and Output operation functions
  *  @brief   HCD IO operation functions
  *
@verbatim
 ===============================================================================
                      ##### IO operation functions #####
 ===============================================================================
 [..] This subsection provides a set of functions allowing to manage the USB Host Data 
    Transfer
       
@endverbatim
  * @{
  */
  
/**                                
  * @brief  Submit a new URB for processing. 
  * @param  hhcd: HCD handle
  * @param  ch_num: Channel number.
  *         This parameter can be a value from 1 to 15
  * @param  direction: Channel number.
  *          This parameter can be one of these values:
  *           0 : Output / 1 : Input
  * @param  ep_type: Endpoint Type.
  *          This parameter can be one of these values:
  *            EP_TYPE_CTRL: Control type/
  *            EP_TYPE_ISOC: Isochronous type/
  *            EP_TYPE_BULK: Bulk type/
  *            EP_TYPE_INTR: Interrupt type/
  * @param  token: Endpoint Type.
  *          This parameter can be one of these values:
  *            0: HC_PID_SETUP / 1: HC_PID_DATA1
  * @param  pbuff: pointer to URB data
  * @param  length: Length of URB data
  * @param  do_ping: activate do ping protocol (for high speed only).
  *          This parameter can be one of these values:
  *           0 : do ping inactive / 1 : do ping active 
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HCD_HC_SubmitRequest(HCD_HandleTypeDef *hhcd,
                                            uint8_t ch_num, 
                                            uint8_t direction ,
                                            uint8_t ep_type,  
                                            uint8_t token, 
                                            uint8_t* pbuff, 
                                            uint16_t length,
                                            uint8_t do_ping) 
{
  hhcd->hc[ch_num].ep_is_in = direction;
  hhcd->hc[ch_num].ep_type  = ep_type; 
  
  if(token == 0)
  {
    hhcd->hc[ch_num].data_pid = HC_PID_SETUP;
  }
  else
  {
    hhcd->hc[ch_num].data_pid = HC_PID_DATA1;
  }
  
  /* Manage Data Toggle */
  switch(ep_type)
  {
  case EP_TYPE_CTRL:
    if((token == 1) && (direction == 0)) /*send data */
    {
      if ( length == 0 )
      { /* For Status OUT stage, Length==0, Status Out PID = 1 */
        hhcd->hc[ch_num].toggle_out = 1;
      }
      
      /* Set the Data Toggle bit as per the Flag */
      if ( hhcd->hc[ch_num].toggle_out == 0)
      { /* Put the PID 0 */
        hhcd->hc[ch_num].data_pid = HC_PID_DATA0;    
      }
      else
      { /* Put the PID 1 */
        hhcd->hc[ch_num].data_pid = HC_PID_DATA1 ;
      }
      if(hhcd->hc[ch_num].urb_state  != URB_NOTREADY)
      {
        hhcd->hc[ch_num].do_ping = do_ping;
      }
    }
    break;
  
  case EP_TYPE_BULK:
    if(direction == 0)
    {
      /* Set the Data Toggle bit as per the Flag */
      if ( hhcd->hc[ch_num].toggle_out == 0)
      { /* Put the PID 0 */
        hhcd->hc[ch_num].data_pid = HC_PID_DATA0;    
      }
      else
      { /* Put the PID 1 */
        hhcd->hc[ch_num].data_pid = HC_PID_DATA1 ;
      }
      if(hhcd->hc[ch_num].urb_state  != URB_NOTREADY)
      {
        hhcd->hc[ch_num].do_ping = do_ping;
      }
    }
    else
    {
      if( hhcd->hc[ch_num].toggle_in == 0)
      {
        hhcd->hc[ch_num].data_pid = HC_PID_DATA0;
      }
      else
      {
        hhcd->hc[ch_num].data_pid = HC_PID_DATA1;
      }
    }
    
    break;
  case EP_TYPE_INTR:
    if(direction == 0)
    {
      /* Set the Data Toggle bit as per the Flag */
      if ( hhcd->hc[ch_num].toggle_out == 0)
      { /* Put the PID 0 */
        hhcd->hc[ch_num].data_pid = HC_PID_DATA0;    
      }
      else
      { /* Put the PID 1 */
        hhcd->hc[ch_num].data_pid = HC_PID_DATA1 ;
      }
    }
    else
    {
      if( hhcd->hc[ch_num].toggle_in == 0)
      {
        hhcd->hc[ch_num].data_pid = HC_PID_DATA0;
      }
      else
      {
        hhcd->hc[ch_num].data_pid = HC_PID_DATA1;
      }
    }
    break;
    
  case EP_TYPE_ISOC: 
    hhcd->hc[ch_num].data_pid = HC_PID_DATA0;
    break;      
  }
  
  hhcd->hc[ch_num].xfer_buff = pbuff;
  hhcd->hc[ch_num].xfer_len  = length;
  hhcd->hc[ch_num].urb_state =   URB_IDLE;  
  hhcd->hc[ch_num].xfer_count = 0 ;
  hhcd->hc[ch_num].ch_num = ch_num;
  hhcd->hc[ch_num].state = HC_IDLE;
  
  return USB_HC_StartXfer(hhcd->Instance, &(hhcd->hc[ch_num]), hhcd->Init.dma_enable);
}

/**
  * @brief  Handle HCD interrupt request.
  * @param  hhcd: HCD handle
  * @retval None
  */
void HAL_HCD_IRQHandler(HCD_HandleTypeDef *hhcd)
{
  USB_OTG_GlobalTypeDef *USBx = hhcd->Instance;
  uint32_t i = 0 , interrupt = 0;
  
  /* ensure that we are in device mode */
  if (USB_GetMode(hhcd->Instance) == USB_OTG_MODE_HOST) // 主机模式
  {
    /* avoid spurious interrupt */
    if(__HAL_HCD_IS_INVALID_INTERRUPT(hhcd)) // USB还未初始化
    {
      return;
    }
    
    // Incomplete periodic transfer，未完成周期传输
    if(__HAL_HCD_GET_FLAG(hhcd, USB_OTG_GINTSTS_PXFR_INCOMPISOOUT))
    {
     /* incorrect mode, acknowledge the interrupt */
      __HAL_HCD_CLEAR_FLAG(hhcd, USB_OTG_GINTSTS_PXFR_INCOMPISOOUT);
    }
    
    // Incomplete isochronous IN transfer，未完成IN同步传输
    if(__HAL_HCD_GET_FLAG(hhcd, USB_OTG_GINTSTS_IISOIXFR))
    {
     /* incorrect mode, acknowledge the interrupt */
      __HAL_HCD_CLEAR_FLAG(hhcd, USB_OTG_GINTSTS_IISOIXFR);
    }

    // Periodic TxFIFO empty，周期性TX FIFO为空
    if(__HAL_HCD_GET_FLAG(hhcd, USB_OTG_GINTSTS_PTXFE))
    {
     /* incorrect mode, acknowledge the interrupt */
      __HAL_HCD_CLEAR_FLAG(hhcd, USB_OTG_GINTSTS_PTXFE);
    }   
    
    // Mode mismatch interrupt，模式不匹配
    if(__HAL_HCD_GET_FLAG(hhcd, USB_OTG_GINTSTS_MMIS))
    {
     /* incorrect mode, acknowledge the interrupt */
      __HAL_HCD_CLEAR_FLAG(hhcd, USB_OTG_GINTSTS_MMIS);
    }     
    
    /* Handle Host Disconnect Interrupts */
    if(__HAL_HCD_GET_FLAG(hhcd, USB_OTG_GINTSTS_DISCINT)) // 检测到断开链接
    {
      /* Cleanup HPRT */
      USBx_HPRT0 &= ~(USB_OTG_HPRT_PENA | USB_OTG_HPRT_PCDET |\
        USB_OTG_HPRT_PENCHNG | USB_OTG_HPRT_POCCHNG ); // 重置主机配置寄存器
       
      /* Handle Host Port Interrupts */
      HAL_HCD_Disconnect_Callback(hhcd); // usb_core.c进行处理
       USB_InitFSLSPClkSel(hhcd->Instance ,HCFG_48_MHZ );
      __HAL_HCD_CLEAR_FLAG(hhcd, USB_OTG_GINTSTS_DISCINT);
    }
    
    /* Handle Host Port Interrupts */
    if(__HAL_HCD_GET_FLAG(hhcd, USB_OTG_GINTSTS_HPRTINT)) // 主机端口终端
    {
      HCD_Port_IRQHandler (hhcd); // 处理端口中断
    }
    
    /* Handle Host SOF Interrupts */
    if(__HAL_HCD_GET_FLAG(hhcd, USB_OTG_GINTSTS_SOF)) // 发送了SOF帧
    {
      HAL_HCD_SOF_Callback(hhcd);
      __HAL_HCD_CLEAR_FLAG(hhcd, USB_OTG_GINTSTS_SOF);
    }
          
    /* Handle Host channel Interrupts */
    if(__HAL_HCD_GET_FLAG(hhcd, USB_OTG_GINTSTS_HCINT)) // 主机通道中断
    {
      interrupt = USB_HC_ReadInterrupt(hhcd->Instance); // 获取通道号
      for (i = 0; i < hhcd->Init.Host_channels ; i++)
      {
        if (interrupt & (1 << i))
        {
          if ((USBx_HC(i)->HCCHAR) &  USB_OTG_HCCHAR_EPDIR) // 获取端点方向
          {
            HCD_HC_IN_IRQHandler (hhcd, i);
          }
          else
          {
            HCD_HC_OUT_IRQHandler (hhcd, i);
          }
        }
      }
      __HAL_HCD_CLEAR_FLAG(hhcd, USB_OTG_GINTSTS_HCINT);
    } 
    
        /* Handle Rx Queue Level Interrupts */
    if(__HAL_HCD_GET_FLAG(hhcd, USB_OTG_GINTSTS_RXFLVL)) // RX FIFO非空，有数据可读
    {
      USB_MASK_INTERRUPT(hhcd->Instance, USB_OTG_GINTSTS_RXFLVL); // 屏蔽中断
      
      HCD_RXQLVL_IRQHandler (hhcd); // 读RX FIFO数据
      
      USB_UNMASK_INTERRUPT(hhcd->Instance, USB_OTG_GINTSTS_RXFLVL); // 使能中断
    }
  }
}

/**
  * @brief  SOF callback.
  * @param  hhcd: HCD handle
  * @retval None
  */
#if 0
__weak void HAL_HCD_SOF_Callback(HCD_HandleTypeDef *hhcd)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hhcd);
  
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_HCD_SOF_Callback could be implemented in the user file
   */
}
#endif
/**
  * @brief Connection Event callback.
  * @param  hhcd: HCD handle
  * @retval None
  */
#if 0
__weak void HAL_HCD_Connect_Callback(HCD_HandleTypeDef *hhcd)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hhcd);
  
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_HCD_Connect_Callback could be implemented in the user file
   */
}
#endif
/**
  * @brief  Disconnection Event callback.
  * @param  hhcd: HCD handle
  * @retval None
  */
#if 0
__weak void HAL_HCD_Disconnect_Callback(HCD_HandleTypeDef *hhcd)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hhcd);
  
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_HCD_Disconnect_Callback could be implemented in the user file
   */
} 
#endif
/**
  * @brief  Notify URB state change callback.
  * @param  hhcd: HCD handle
  * @param  chnum: Channel number.
  *         This parameter can be a value from 1 to 15
  * @param  urb_state:
  *          This parameter can be one of these values:
  *            URB_IDLE/
  *            URB_DONE/
  *            URB_NOTREADY/
  *            URB_NYET/ 
  *            URB_ERROR/  
  *            URB_STALL/    
  * @retval None
  */
__weak void HAL_HCD_HC_NotifyURBChange_Callback(HCD_HandleTypeDef *hhcd, uint8_t chnum, HCD_URBStateTypeDef urb_state)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hhcd);
  
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_HCD_HC_NotifyURBChange_Callback could be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup HCD_Exported_Functions_Group3 Peripheral Control functions 
 *  @brief   Management functions 
 *
@verbatim   
 ===============================================================================
                      ##### Peripheral Control functions #####
 ===============================================================================  
    [..]
    This subsection provides a set of functions allowing to control the HCD data 
    transfers.

@endverbatim
  * @{
  */

/**
  * @brief  Start the host driver.
  * @param  hhcd: HCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HCD_Start(HCD_HandleTypeDef *hhcd)
{ 
  __HAL_LOCK(hhcd); // TODO:获取锁
  __HAL_HCD_ENABLE(hhcd); // TODO:开启中断
  USB_DriveVbus(hhcd->Instance, 1); // 上电
  __HAL_UNLOCK(hhcd); // 解锁
  return HAL_OK;
}

/**
  * @brief  Stop the host driver.
  * @param  hhcd: HCD handle
  * @retval HAL status
  */

HAL_StatusTypeDef HAL_HCD_Stop(HCD_HandleTypeDef *hhcd)
{ 
  __HAL_LOCK(hhcd); 
  USB_StopHost(hhcd->Instance); // 刷新FIFO，清通道，清中断
  __HAL_UNLOCK(hhcd); 
  return HAL_OK;
}

/**
  * @brief  Reset the host port.
  * @param  hhcd: HCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_HCD_ResetPort(HCD_HandleTypeDef *hhcd)
{
  return (USB_ResetPort(hhcd->Instance)); // 端口复位
}

/**
  * @}
  */

/** @defgroup HCD_Exported_Functions_Group4 Peripheral State functions 
 *  @brief   Peripheral State functions 
 *
@verbatim   
 ===============================================================================
                      ##### Peripheral State functions #####
 ===============================================================================  
    [..]
    This subsection permits to get in run-time the status of the peripheral 
    and the data flow.

@endverbatim
  * @{
  */

/**
  * @brief  Return the HCD handle state.
  * @param  hhcd: HCD handle
  * @retval HAL state
  */
HCD_StateTypeDef HAL_HCD_GetState(HCD_HandleTypeDef *hhcd)
{
  return hhcd->State;
}

/**
  * @brief  Return  URB state for a channel.
  * @param  hhcd: HCD handle
  * @param  chnum: Channel number.
  *         This parameter can be a value from 1 to 15
  * @retval URB state.
  *          This parameter can be one of these values:
  *            URB_IDLE/
  *            URB_DONE/
  *            URB_NOTREADY/
  *            URB_NYET/ 
  *            URB_ERROR/  
  *            URB_STALL
  */
HCD_URBStateTypeDef HAL_HCD_HC_GetURBState(HCD_HandleTypeDef *hhcd, uint8_t chnum)
{
  return hhcd->hc[chnum].urb_state;
}


/**
  * @brief  Return the last host transfer size.
  * @param  hhcd: HCD handle
  * @param  chnum: Channel number.
  *         This parameter can be a value from 1 to 15
  * @retval last transfer size in byte
  */
uint32_t HAL_HCD_HC_GetXferCount(HCD_HandleTypeDef *hhcd, uint8_t chnum)
{
  return hhcd->hc[chnum].xfer_count; 
}
  
/**
  * @brief  Return the Host Channel state.
  * @param  hhcd: HCD handle
  * @param  chnum: Channel number.
  *         This parameter can be a value from 1 to 15
  * @retval Host channel state
  *          This parameter can be one of these values:
  *            HC_IDLE/
  *            HC_XFRC/
  *            HC_HALTED/
  *            HC_NYET/ 
  *            HC_NAK/  
  *            HC_STALL/ 
  *            HC_XACTERR/  
  *            HC_BBLERR/  
  *            HC_DATATGLERR   
  */
HCD_HCStateTypeDef  HAL_HCD_HC_GetState(HCD_HandleTypeDef *hhcd, uint8_t chnum)
{
  return hhcd->hc[chnum].state;
}

/**
  * @brief  Return the current Host frame number.
  * @param  hhcd: HCD handle
  * @retval Current Host frame number
  */
uint32_t HAL_HCD_GetCurrentFrame(HCD_HandleTypeDef *hhcd)
{
  return (USB_GetCurrentFrame(hhcd->Instance));
}

/**
  * @brief  Return the Host enumeration speed.
  * @param  hhcd: HCD handle
  * @retval Enumeration speed
  */
uint32_t HAL_HCD_GetCurrentSpeed(HCD_HandleTypeDef *hhcd)
{
  return (USB_GetHostSpeed(hhcd->Instance));
}

/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup HCD_Private_Functions
  * @{
  */
/**
  * @brief  Handle Host Channel IN interrupt requests.
  * @param  hhcd: HCD handle
  * @param  chnum: Channel number.
  *         This parameter can be a value from 1 to 15
  * @retval none
  */
static void HCD_HC_IN_IRQHandler   (HCD_HandleTypeDef *hhcd, uint8_t chnum)
{
  USB_OTG_GlobalTypeDef *USBx = hhcd->Instance;
  uint32_t tmpreg = 0;
  
  if ((USBx_HC(chnum)->HCINT) &  USB_OTG_HCINT_AHBERR) // （高速模式下）
  {
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_AHBERR);
    __HAL_HCD_UNMASK_HALT_HC_INT(chnum);
  }  
  else if ((USBx_HC(chnum)->HCINT) &  USB_OTG_HCINT_ACK) // 收到/发出ACK响应中断
  {
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_ACK);
  }
  else if ((USBx_HC(chnum)->HCINT) &  USB_OTG_HCINT_STALL) // 收到STALL响应中断
  {
    __HAL_HCD_UNMASK_HALT_HC_INT(chnum);
    hhcd->hc[chnum].state = HC_STALL;
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_NAK); // 清NAK中断
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_STALL); // 清STALL中断
    USB_HC_Halt(hhcd->Instance, chnum); // 禁止通道
  }
  else if ((USBx_HC(chnum)->HCINT) &  USB_OTG_HCINT_DTERR) // 数据同步错误
  {
    __HAL_HCD_UNMASK_HALT_HC_INT(chnum); // 使能通道停止中断，其他中断都屏蔽
    USB_HC_Halt(hhcd->Instance, chnum); // 禁止通道
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_NAK); // 清NAK中断
    hhcd->hc[chnum].state = HC_DATATGLERR;
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_DTERR);
  }    
  
  if ((USBx_HC(chnum)->HCINT) &  USB_OTG_HCINT_FRMOR) // 帧溢出
  {
    __HAL_HCD_UNMASK_HALT_HC_INT(chnum); // 使能通道停止中断，其他中断都屏蔽
    USB_HC_Halt(hhcd->Instance, chnum); // 禁止通道
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_FRMOR);
  }
  else if ((USBx_HC(chnum)->HCINT) &  USB_OTG_HCINT_XFRC) // 传输完成
  {
    if (hhcd->Init.dma_enable)
    {
      hhcd->hc[chnum].xfer_count = hhcd->hc[chnum].xfer_len - \
                               (USBx_HC(chnum)->HCTSIZ & USB_OTG_HCTSIZ_XFRSIZ); // 剩余发送字节数
    }
    
    hhcd->hc[chnum].state = HC_XFRC;
    hhcd->hc[chnum].ErrCnt = 0;
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_XFRC);
    
    
    if ((hhcd->hc[chnum].ep_type == EP_TYPE_CTRL)||
        (hhcd->hc[chnum].ep_type == EP_TYPE_BULK)) // 控制或批量
    {
      __HAL_HCD_UNMASK_HALT_HC_INT(chnum); // 使能通道停止中断，其他中断都屏蔽
      USB_HC_Halt(hhcd->Instance, chnum); // 禁止通道
      __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_NAK);
      
    }
    else if(hhcd->hc[chnum].ep_type == EP_TYPE_INTR) // 中断传输
    {
      USBx_HC(chnum)->HCCHAR |= USB_OTG_HCCHAR_ODDFRM; // 奇数帧
      hhcd->hc[chnum].urb_state = URB_DONE; 
      HAL_HCD_HC_NotifyURBChange_Callback(hhcd, chnum, hhcd->hc[chnum].urb_state);
    }
    hhcd->hc[chnum].toggle_in ^= 1;
    
  }
  else if ((USBx_HC(chnum)->HCINT) &  USB_OTG_HCINT_CHH) // 禁止通道
  {
    __HAL_HCD_MASK_HALT_HC_INT(chnum); // 屏蔽通道停止中断
    
    if(hhcd->hc[chnum].state == HC_XFRC) // 传输已完成
    {
      hhcd->hc[chnum].urb_state  = URB_DONE;      
    }
    
    else if (hhcd->hc[chnum].state == HC_STALL) // 传输停止
    {
      hhcd->hc[chnum].urb_state  = URB_STALL;
    }   
    
    else if((hhcd->hc[chnum].state == HC_XACTERR) ||
            (hhcd->hc[chnum].state == HC_DATATGLERR)) // 发哦错误
    {
      if(hhcd->hc[chnum].ErrCnt++ > 3)
      {      
        hhcd->hc[chnum].ErrCnt = 0;
        hhcd->hc[chnum].urb_state = URB_ERROR;
      }
      else
      {
        hhcd->hc[chnum].urb_state = URB_NOTREADY;
      }
      
      /* re-activate the channel  */
      tmpreg = USBx_HC(chnum)->HCCHAR;
      tmpreg &= ~USB_OTG_HCCHAR_CHDIS;
      tmpreg |= USB_OTG_HCCHAR_CHENA; // 使能通道
      USBx_HC(chnum)->HCCHAR = tmpreg;    
    }
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_CHH);
    HAL_HCD_HC_NotifyURBChange_Callback(hhcd, chnum, hhcd->hc[chnum].urb_state);
  }  
  else if ((USBx_HC(chnum)->HCINT) &  USB_OTG_HCINT_TXERR) // 通信事务错误
  {
    __HAL_HCD_UNMASK_HALT_HC_INT(chnum); // 使能通道停止中断，其他中断都屏蔽
     hhcd->hc[chnum].ErrCnt++;
     hhcd->hc[chnum].state = HC_XACTERR;
     USB_HC_Halt(hhcd->Instance, chnum); // 停止通道
     __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_TXERR);
  }
  else if ((USBx_HC(chnum)->HCINT) &  USB_OTG_HCINT_NAK) // 收到NAK响应
  {  
    if(hhcd->hc[chnum].ep_type == EP_TYPE_INTR) // 中断模式
    {
      __HAL_HCD_UNMASK_HALT_HC_INT(chnum); // 使能通道停止中断，其他中断都屏蔽
      USB_HC_Halt(hhcd->Instance, chnum); // 停止通道
    }
    else if  ((hhcd->hc[chnum].ep_type == EP_TYPE_CTRL)||
         (hhcd->hc[chnum].ep_type == EP_TYPE_BULK)) // 控制或批量模式
    {
      /* re-activate the channel  */
      tmpreg = USBx_HC(chnum)->HCCHAR;
      tmpreg &= ~USB_OTG_HCCHAR_CHDIS;
      tmpreg |= USB_OTG_HCCHAR_CHENA;
      USBx_HC(chnum)->HCCHAR = tmpreg;
    }
    hhcd->hc[chnum].state = HC_NAK;
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_NAK);
  }
}

/**
  * @brief  Handle Host Channel OUT interrupt requests.
  * @param  hhcd: HCD handle
  * @param  chnum: Channel number.
  *         This parameter can be a value from 1 to 15
  * @retval none
  */
static void HCD_HC_OUT_IRQHandler  (HCD_HandleTypeDef *hhcd, uint8_t chnum)
{
  USB_OTG_GlobalTypeDef *USBx = hhcd->Instance;
  uint32_t tmpreg = 0;
  
  if ((USBx_HC(chnum)->HCINT) &  USB_OTG_HCINT_AHBERR) //
  {
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_AHBERR);
    __HAL_HCD_UNMASK_HALT_HC_INT(chnum); // 使能通道停止中断，其他中断都屏蔽
  }  

  else if ((USBx_HC(chnum)->HCINT) &  USB_OTG_HCINT_ACK) // 收到或者发出ACK
  {
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_ACK);
    
    if( hhcd->hc[chnum].do_ping == 1)
    {
      hhcd->hc[chnum].state = HC_NYET;     
      __HAL_HCD_UNMASK_HALT_HC_INT(chnum); // 使能通道停止中断，其他中断都屏蔽
      USB_HC_Halt(hhcd->Instance, chnum); // 停止通道
      hhcd->hc[chnum].urb_state  = URB_NOTREADY;
    }
  }
  
  else if ((USBx_HC(chnum)->HCINT) &  USB_OTG_HCINT_NYET) // （HS模式下）收到尚未就绪
  {
    hhcd->hc[chnum].state = HC_NYET;
    hhcd->hc[chnum].ErrCnt= 0;    
    __HAL_HCD_UNMASK_HALT_HC_INT(chnum); // 使能通道停止中断，其他中断都屏蔽
    USB_HC_Halt(hhcd->Instance, chnum); // 停止通道
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_NYET);
    
  }  
  
  else if ((USBx_HC(chnum)->HCINT) &  USB_OTG_HCINT_FRMOR) // 帧溢出
  {
    __HAL_HCD_UNMASK_HALT_HC_INT(chnum); // 使能通道停止中断，其他中断都屏蔽
    USB_HC_Halt(hhcd->Instance, chnum);
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_FRMOR);
  }
  
  else if ((USBx_HC(chnum)->HCINT) &  USB_OTG_HCINT_XFRC) // 传输完成
  {
      hhcd->hc[chnum].ErrCnt = 0;  
    __HAL_HCD_UNMASK_HALT_HC_INT(chnum); // 使能通道停止中断，其他中断都屏蔽
    USB_HC_Halt(hhcd->Instance, chnum);   
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_XFRC);
    hhcd->hc[chnum].state = HC_XFRC;
  }  

  else if ((USBx_HC(chnum)->HCINT) &  USB_OTG_HCINT_STALL) // 收到STALL响应
  {
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_STALL);  
    __HAL_HCD_UNMASK_HALT_HC_INT(chnum);
    USB_HC_Halt(hhcd->Instance, chnum);   
    hhcd->hc[chnum].state = HC_STALL;    
  }

  else if ((USBx_HC(chnum)->HCINT) &  USB_OTG_HCINT_NAK) // 收到NAK响应
  {  
    hhcd->hc[chnum].ErrCnt = 0;  
    __HAL_HCD_UNMASK_HALT_HC_INT(chnum); 
    USB_HC_Halt(hhcd->Instance, chnum);   
    hhcd->hc[chnum].state = HC_NAK;
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_NAK);
  }

  else if ((USBx_HC(chnum)->HCINT) &  USB_OTG_HCINT_TXERR) // 通信事务错误
  {
    __HAL_HCD_UNMASK_HALT_HC_INT(chnum); 
    USB_HC_Halt(hhcd->Instance, chnum);      
    hhcd->hc[chnum].state = HC_XACTERR;  
     __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_TXERR);
  }
  
  else if ((USBx_HC(chnum)->HCINT) &  USB_OTG_HCINT_DTERR) // 数据同步错误
  {
    __HAL_HCD_UNMASK_HALT_HC_INT(chnum); // 使能通道停止中断，其他中断都屏蔽
    USB_HC_Halt(hhcd->Instance, chnum);      
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_NAK);
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_DTERR);    
    hhcd->hc[chnum].state = HC_DATATGLERR;
  }
  
  else if ((USBx_HC(chnum)->HCINT) &  USB_OTG_HCINT_CHH) // 通道停止
  {
    __HAL_HCD_MASK_HALT_HC_INT(chnum); // 禁止使能通道中断
    
    if(hhcd->hc[chnum].state == HC_XFRC)
    {
      hhcd->hc[chnum].urb_state  = URB_DONE;
      if (hhcd->hc[chnum].ep_type == EP_TYPE_BULK)
      {
        hhcd->hc[chnum].toggle_out ^= 1; 
      }      
    }

    else if (hhcd->hc[chnum].state == HC_NAK) 
    {
      hhcd->hc[chnum].urb_state  = URB_NOTREADY;
    }  
    
    else if (hhcd->hc[chnum].state == HC_NYET) 
    {
      hhcd->hc[chnum].urb_state  = URB_NOTREADY;
      hhcd->hc[chnum].do_ping = 0;
    }   
    
    else if (hhcd->hc[chnum].state == HC_STALL) 
    {
      hhcd->hc[chnum].urb_state  = URB_STALL;
    } 
    
    else if((hhcd->hc[chnum].state == HC_XACTERR) ||
            (hhcd->hc[chnum].state == HC_DATATGLERR)) // 发送错误
    {
      if(hhcd->hc[chnum].ErrCnt++ > 3)
      {      
        hhcd->hc[chnum].ErrCnt = 0;
        hhcd->hc[chnum].urb_state = URB_ERROR;
      }
      else
      {
        hhcd->hc[chnum].urb_state = URB_NOTREADY;
      }
      
      /* re-activate the channel  */
      tmpreg = USBx_HC(chnum)->HCCHAR;
      tmpreg &= ~USB_OTG_HCCHAR_CHDIS;
      tmpreg |= USB_OTG_HCCHAR_CHENA;
      USBx_HC(chnum)->HCCHAR = tmpreg;   
    }
    
    __HAL_HCD_CLEAR_HC_INT(chnum, USB_OTG_HCINT_CHH);
    HAL_HCD_HC_NotifyURBChange_Callback(hhcd, chnum, hhcd->hc[chnum].urb_state);  
  }
} 

/**
  * @brief  Handle Rx Queue Level interrupt requests.
  * @param  hhcd: HCD handle
  * @retval none
  */
static void HCD_RXQLVL_IRQHandler  (HCD_HandleTypeDef *hhcd)
{
  USB_OTG_GlobalTypeDef *USBx = hhcd->Instance;  
  uint8_t                       channelnum =0;  
  uint32_t                      pktsts;
  uint32_t                      pktcnt; 
  uint32_t                      temp = 0;
  uint32_t tmpreg = 0;
  
  temp = hhcd->Instance->GRXSTSP ;
  channelnum = temp &  USB_OTG_GRXSTSP_EPNUM; // 当前接收的数据包所属通道
  pktsts = (temp &  USB_OTG_GRXSTSP_PKTSTS) >> 17; // 数据包状态
  pktcnt = (temp &  USB_OTG_GRXSTSP_BCNT) >> 4; // 包的字节数
    
  switch (pktsts)
  {
  case GRXSTS_PKTSTS_IN: // 接收的数据包的状态
    /* Read the data into the host buffer. */
    if ((pktcnt > 0) && (hhcd->hc[channelnum].xfer_buff != (void  *)0)) // 存在缓冲
    {  
      
      USB_ReadPacket(hhcd->Instance, hhcd->hc[channelnum].xfer_buff, pktcnt); // 读RX FIFO
     
      /*manage multiple Xfer */
      hhcd->hc[channelnum].xfer_buff += pktcnt;           
      hhcd->hc[channelnum].xfer_count  += pktcnt;
        
      if((USBx_HC(channelnum)->HCTSIZ & USB_OTG_HCTSIZ_PKTCNT) > 0) // 数据包计数
      {
        /* re-activate the channel when more packets are expected */
        tmpreg = USBx_HC(channelnum)->HCCHAR;
        tmpreg &= ~USB_OTG_HCCHAR_CHDIS;
        tmpreg |= USB_OTG_HCCHAR_CHENA;
        USBx_HC(channelnum)->HCCHAR = tmpreg; // 重新激活通道
        hhcd->hc[channelnum].toggle_in ^= 1;
      }
    }
    break;

  case GRXSTS_PKTSTS_DATA_TOGGLE_ERR: // 数据同步错误
    break;
  case GRXSTS_PKTSTS_IN_XFER_COMP: // IN传输完成
  case GRXSTS_PKTSTS_CH_HALTED: // 暂停通道
  default:
    break;
  }
}

/**
  * @brief  Handle Host Port interrupt requests.
  * @param  hhcd: HCD handle
  * @retval None
  */
static void HCD_Port_IRQHandler  (HCD_HandleTypeDef *hhcd)
{
  USB_OTG_GlobalTypeDef *USBx = hhcd->Instance;  
  __IO uint32_t hprt0, hprt0_dup;
  
  /* Handle Host Port Interrupts */
  hprt0 = USBx_HPRT0;
  hprt0_dup = USBx_HPRT0;
  
  hprt0_dup &= ~(USB_OTG_HPRT_PENA | USB_OTG_HPRT_PCDET |\
                 USB_OTG_HPRT_PENCHNG | USB_OTG_HPRT_POCCHNG ); // 禁止PENA(端口)| 端口链接 变化| PENA位变化 | 过流变化
  
  /* Check whether Port Connect detected */
  if((hprt0 & USB_OTG_HPRT_PCDET) == USB_OTG_HPRT_PCDET) // 端口链接变化
  {  
    if((hprt0 & USB_OTG_HPRT_PCSTS) == USB_OTG_HPRT_PCSTS) // 链接
    {
      USB_MASK_INTERRUPT(hhcd->Instance, USB_OTG_GINTSTS_DISCINT);
      HAL_HCD_Connect_Callback(hhcd);
    }
    hprt0_dup  |= USB_OTG_HPRT_PCDET;
    
  }
  
  /* Check whether Port Enable Changed */
  if((hprt0 & USB_OTG_HPRT_PENCHNG) == USB_OTG_HPRT_PENCHNG) // 端口使能位发生变化
  {
    hprt0_dup |= USB_OTG_HPRT_PENCHNG; // TODO
    
    if((hprt0 & USB_OTG_HPRT_PENA) == USB_OTG_HPRT_PENA) // 使能端口
    {    
      if(hhcd->Init.phy_itface  == USB_OTG_EMBEDDED_PHY) // 嵌入式的USB PHY
      {
        if ((hprt0 & USB_OTG_HPRT_PSPD) == (HPRT0_PRTSPD_LOW_SPEED << 17))
        {
          USB_InitFSLSPClkSel(hhcd->Instance ,HCFG_6_MHZ ); // 低速
        }
        else
        {
          USB_InitFSLSPClkSel(hhcd->Instance ,HCFG_48_MHZ ); // 高速
        }
      }
      else // 外置的USB PHY
      {
        if(hhcd->Init.speed == HCD_SPEED_FULL)
        {
          USBx_HOST->HFIR = (uint32_t)60000;
        }
      }
      HAL_HCD_Connect_Callback(hhcd);
      
    }
    else // 禁止端口
    {
      /* Cleanup HPRT */
      USBx_HPRT0 &= ~(USB_OTG_HPRT_PENA | USB_OTG_HPRT_PCDET |\
        USB_OTG_HPRT_PENCHNG | USB_OTG_HPRT_POCCHNG );
      
      USB_UNMASK_INTERRUPT(hhcd->Instance, USB_OTG_GINTSTS_DISCINT); // 屏蔽检测到断开链接中断
    }    
  }
  
  /* Check For an overcurrent */
  if((hprt0 & USB_OTG_HPRT_POCCHNG) == USB_OTG_HPRT_POCCHNG) // 过流
  {
    hprt0_dup |= USB_OTG_HPRT_POCCHNG;
  }

  /* Clear Port Interrupts */
  USBx_HPRT0 = hprt0_dup;
}

/**
  * @}
  */

/**
  * @}
  */
  
#endif /* HAL_HCD_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
