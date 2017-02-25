/**
  ******************************************************************************
  * @file    USB_Host/MSC_Standalone/Src/usbh_conf.c
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    22-April-2016
  * @brief   USB Host configuration file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"
#include "../core/inc/usbh_core.h"

HCD_HandleTypeDef hhcd;

/*******************************************************************************
                       HCD BSP Routines
*******************************************************************************/
/**
  * @brief  Initializes the HCD MSP.
  * @param  hhcd: HCD handle
  * @retval None
  */
void HAL_HCD_MspInit(HCD_HandleTypeDef *hhcd)
{
  GPIO_InitTypeDef  GPIO_InitStruct;
#if 0
  /* Configure USB FS GPIOs */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  
  /*USB DM and DP */    
  GPIO_InitStruct.Pin = (GPIO_PIN_11 | GPIO_PIN_12);
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 
  
  /*USB ID */    
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 
  
  /* Configure POWER_SWITCH IO pin */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct); 
  
  /* Enable USB FS Clocks */ 
  __HAL_RCC_USB_OTG_FS_CLK_ENABLE();
#else // TODO
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* Configure Over Current IO pin */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /* Configure POWER_SWITCH IO pin */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /* Configure GPIO for HS in FS mode */
  GPIO_InitStruct.Pin = GPIO_PIN_13 | GPIO_PIN_14 |GPIO_PIN_15; // PB12=ID 未用,PB13=VBUS_FS2(TODO),PB14=DM,PB15=DP
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_OTG_HS_FS;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  USBH_LL_DriverVBUS(hhcd, (u8)0);
  // 确保USB设备完全掉电
  for(;;)
  {
    if(GPIO_PIN_SET == HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13))
	  Djy_EventDelay(100);
    else
      break;
  }
  /* Enable USB HS Clocks */
  __HAL_RCC_USB_OTG_HS_CLK_ENABLE();
#endif

#if 0
  /* Set USBFS Interrupt priority */
  HAL_NVIC_SetPriority(OTG_FS_IRQn, 6, 0);
  
  /* Enable USBFS Interrupt */
  HAL_NVIC_EnableIRQ(OTG_FS_IRQn);
#endif
}

/**
  * @brief  DeInitializes the HCD MSP.
  * @param  hhcd: HCD handle
  * @retval None
  */
void HAL_HCD_MspDeInit(HCD_HandleTypeDef *hhcd)
{
#if 0
  /* Disable USB FS Clocks */ 
  __HAL_RCC_USB_OTG_FS_CLK_DISABLE();
#else // TODO
  /* Disable USB HS Clocks */
  __HAL_RCC_USB_OTG_HS_CLK_DISABLE();
  __HAL_RCC_USB_OTG_HS_ULPI_CLK_DISABLE();
#endif
}

/*******************************************************************************
                       LL Driver Callbacks (HCD -> USB Host Library)
*******************************************************************************/

/**
  * @brief  SOF callback.
  * @param  hhcd: HCD handle
  * @retval None
  */
void HAL_HCD_SOF_Callback(HCD_HandleTypeDef *hhcd)
{
  USBH_LL_IncTimer (hhcd->pData); // usb_core.c
}

/**
  * @brief  Connect callback.
  * @param  hhcd: HCD handle
  * @retval None
  */
void HAL_HCD_Connect_Callback(HCD_HandleTypeDef *hhcd)
{
  USBH_LL_Connect(hhcd->pData); // usb_core.c
}

/**
  * @brief  Disconnect callback.
  * @param  hhcd: HCD handle
  * @retval None
  */
void HAL_HCD_Disconnect_Callback(HCD_HandleTypeDef *hhcd)
{
  USBH_LL_Disconnect(hhcd->pData); // usb_core.c
} 


/**
  * @brief  Notify URB state change callback.
  * @param  hhcd: HCD handle
  * @param  chnum: Channel number 
  * @param  urb_state: URB State
  * @retval None
  */
void HAL_HCD_HC_NotifyURBChange_Callback(HCD_HandleTypeDef *hhcd, uint8_t chnum, HCD_URBStateTypeDef urb_state)
{
  /* To be used with OS to sync URB state with the global state machine */
  // URB状态发生变化后的回调函数
}

/*******************************************************************************
                       LL Driver Interface (USB Host Library --> HCD)
*******************************************************************************/
/**
  * @brief  USBH_LL_Init 
  *         Initialize the Low Level portion of the Host driver.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_LL_Init(USBH_HandleTypeDef *phost)
{
#if 0
  /* Set the LL Driver parameters */
  hhcd.Instance = USB_OTG_FS; // FS基地址
  hhcd.Init.Host_channels = 11; 
  hhcd.Init.dma_enable = 0; // DMA未使用
  hhcd.Init.low_power_enable = 0;
  hhcd.Init.phy_itface = HCD_PHY_EMBEDDED; // 接口类型
  hhcd.Init.Sof_enable = 0;
  hhcd.Init.speed = HCD_SPEED_FULL;
  hhcd.Init.vbus_sensing_enable = 0;
  hhcd.Init.lpm_enable = 0;
  
  /* Link the driver to the stack */
  hhcd.pData = phost;
  phost->pData = &hhcd;
  
  /* Initialize the LL Driver */
  HAL_HCD_Init(&hhcd); //
  
  USBH_LL_SetTimer(phost, HAL_HCD_GetCurrentFrame(&hhcd));
  
  return USBH_OK;
#else // TODO
  /* Set the LL driver parameters */
  hhcd.Instance = USB_OTG_HS; // HS基地址
  hhcd.Init.Host_channels = 11;
  hhcd.Init.dma_enable = 0;
  hhcd.Init.low_power_enable = 0;
  hhcd.Init.phy_itface = HCD_PHY_EMBEDDED;
  hhcd.Init.Sof_enable = 0;
  hhcd.Init.speed = HCD_SPEED_HIGH;
  hhcd.Init.vbus_sensing_enable = 0;
  hhcd.Init.use_external_vbus = 1;
  hhcd.Init.lpm_enable = 0;
  /* Link the driver to the stack */
  hhcd.pData = phost;
  phost->pData = &hhcd;

  /* Initialize the LL driver */
  HAL_HCD_Init(&hhcd);

  USBH_LL_SetTimer(phost, HAL_HCD_GetCurrentFrame(&hhcd));

  return USBH_OK;
#endif
}

/**
  * @brief  De-Initializes the Low Level portion of the Host driver.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_LL_DeInit(USBH_HandleTypeDef *phost)
{
  HAL_HCD_DeInit(phost->pData);
  return USBH_OK; 
}

/**
  * @brief  Starts the Low Level portion of the Host driver.   
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_LL_Start(USBH_HandleTypeDef *phost)
{
  HAL_HCD_Start(phost->pData); // 开中断，Vbus上电
  return USBH_OK; 
}

/**
  * @brief  Stops the Low Level portion of the Host driver.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_LL_Stop(USBH_HandleTypeDef *phost)
{
  HAL_HCD_Stop(phost->pData); // 刷新FIFO，清通道，清中断
  return USBH_OK; 
}

/**
  * @brief  Returns the USB Host Speed from the Low Level Driver.
  * @param  phost: Host handle
  * @retval USBH Speeds
  */
USBH_SpeedTypeDef USBH_LL_GetSpeed(USBH_HandleTypeDef *phost)
{
  USBH_SpeedTypeDef speed = USBH_SPEED_FULL;
  
  switch (HAL_HCD_GetCurrentSpeed(phost->pData))
  {
  case 0: 
    speed = USBH_SPEED_HIGH;
    break;
    
  case 1: 
    speed = USBH_SPEED_FULL;
    break;
    
  case 2: 
    speed = USBH_SPEED_LOW;
    break;
    
  default:
    speed = USBH_SPEED_FULL;
    break;
  }
  return speed;
}

/**
  * @brief  Resets the Host Port of the Low Level Driver.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_LL_ResetPort (USBH_HandleTypeDef *phost)
{
  HAL_HCD_ResetPort(phost->pData); // 端口复位
  return USBH_OK; 
}

/**
  * @brief  Returns the last transferred packet size.
  * @param  phost: Host handle
  * @param  pipe: Pipe index   
  * @retval Packet Size
  */
uint32_t USBH_LL_GetLastXferSize(USBH_HandleTypeDef *phost, uint8_t pipe)
{
  return HAL_HCD_HC_GetXferCount(phost->pData, pipe);
}

/**
  * @brief  Opens a pipe of the Low Level Driver.
  * @param  phost: Host handle
  * @param  pipe: Pipe index
  * @param  epnum: Endpoint Number
  * @param  dev_address: Device USB address
  * @param  speed: Device Speed 
  * @param  ep_type: Endpoint Type
  * @param  mps: Endpoint Max Packet Size
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_LL_OpenPipe(USBH_HandleTypeDef *phost, 
                                    uint8_t pipe,
                                    uint8_t epnum,
                                    uint8_t dev_address,
                                    uint8_t speed,
                                    uint8_t ep_type,
                                    uint16_t mps)
{
  HAL_HCD_HC_Init(phost->pData,
                  pipe,
                  epnum,
                  dev_address,
                  speed,
                  ep_type,
                  mps);
  return USBH_OK; 
}

/**
  * @brief  Closes a pipe of the Low Level Driver.
  * @param  phost: Host handle
  * @param  pipe: Pipe index
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_LL_ClosePipe(USBH_HandleTypeDef *phost, uint8_t pipe)
{
  HAL_HCD_HC_Halt(phost->pData, pipe);
  return USBH_OK;
}

/**
  * @brief  Submits a new URB to the low level driver.
  * @param  phost: Host handle
  * @param  pipe: Pipe index    
  *          This parameter can be a value from 1 to 15
  * @param  direction: Channel number
  *          This parameter can be one of these values:
  *           0: Output 
  *           1: Input
  * @param  ep_type: Endpoint Type
  *          This parameter can be one of these values:
  *            @arg EP_TYPE_CTRL: Control type
  *            @arg EP_TYPE_ISOC: Isochronous type
  *            @arg EP_TYPE_BULK: Bulk type
  *            @arg EP_TYPE_INTR: Interrupt type
  * @param  token: Endpoint Type
  *          This parameter can be one of these values:
  *            @arg 0: PID_SETUP
  *            @arg 1: PID_DATA
  * @param  pbuff: pointer to URB data
  * @param  length: length of URB data
  * @param  do_ping: activate do ping protocol (for high speed only)
  *          This parameter can be one of these values:
  *           0: do ping inactive 
  *           1: do ping active 
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_LL_SubmitURB(USBH_HandleTypeDef *phost, 
                                     uint8_t pipe, 
                                     uint8_t direction,
                                     uint8_t ep_type,  
                                     uint8_t token, 
                                     uint8_t* pbuff, 
                                     uint16_t length,
                                     uint8_t do_ping) 
{
  HAL_HCD_HC_SubmitRequest(phost->pData,
                           pipe, 
                           direction,
                           ep_type,  
                           token, 
                           pbuff, 
                           length,
                           do_ping);
  return USBH_OK;   
}

/**
  * @brief  Gets a URB state from the low level driver.
  * @param  phost: Host handle
  * @param  pipe: Pipe index
  *          This parameter can be a value from 1 to 15
  * @retval URB state
  *          This parameter can be one of these values:
  *            @arg URB_IDLE
  *            @arg URB_DONE
  *            @arg URB_NOTREADY
  *            @arg URB_NYET 
  *            @arg URB_ERROR  
  *            @arg URB_STALL      
  */
USBH_URBStateTypeDef USBH_LL_GetURBState(USBH_HandleTypeDef *phost, uint8_t pipe) 
{
  return (USBH_URBStateTypeDef)HAL_HCD_HC_GetURBState (phost->pData, pipe);
}

/**
  * @brief  Drives VBUS.
  * @param  phost: Host handle
  * @param  state: VBUS state
  *          This parameter can be one of these values:
  *           0: VBUS Active 
  *           1: VBUS Inactive
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_LL_DriverVBUS(USBH_HandleTypeDef *phost, uint8_t state)
{
#if 0
  if(state == 0)
  {
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_RESET);
  }
  else
  {
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_SET);
  }
  HAL_Delay(200);
  return USBH_OK;  
#else // TODO
  if(state == 0)
  {
    /* Configure Low Charge pump */
	HAL_GPIO_WritePin(GPIOH, GPIO_PIN_3, GPIO_PIN_SET);
  }
  else
  {
    /* Drive High Charge pump */
	  HAL_GPIO_WritePin(GPIOH, GPIO_PIN_3, GPIO_PIN_RESET);
  }
  HAL_Delay(200);
  return USBH_OK;
#endif
}

/**
  * @brief  Sets toggle for a pipe.
  * @param  phost: Host handle
  * @param  pipe: Pipe index   
  * @param  toggle: toggle (0/1)
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_LL_SetToggle(USBH_HandleTypeDef *phost, uint8_t pipe, uint8_t toggle)
{
  if(hhcd.hc[pipe].ep_is_in)
  {
    hhcd.hc[pipe].toggle_in = toggle;
  }
  else
  {
    hhcd.hc[pipe].toggle_out = toggle;
  }
  return USBH_OK; 
}

/**
  * @brief  Returns the current toggle of a pipe.
  * @param  phost: Host handle
  * @param  pipe: Pipe index
  * @retval toggle (0/1)
  */
uint8_t USBH_LL_GetToggle(USBH_HandleTypeDef *phost, uint8_t pipe)
{
  uint8_t toggle = 0;
  
  if(hhcd.hc[pipe].ep_is_in)
  {
    toggle = hhcd.hc[pipe].toggle_in;
  }
  else
  {
    toggle = hhcd.hc[pipe].toggle_out;
  }
  return toggle; 
}

/**
  * @brief  Delay routine for the USB Host Library
  * @param  Delay: Delay in ms
  * @retval None
  */
void USBH_Delay(uint32_t Delay)
{
#if (USBH_USE_OS == 1)
  osDelay(Delay);
#else
  // HAL_Delay(Delay);
  Djy_EventDelay(Delay); // TODO
#endif  
}

/**
  * @brief  This function handles USB-On-The-Go FS global interrupt request.
  * @param  None
  * @retval None
  */
u32 OTG_FS_IRQHandler(u32 Data)
{
  HAL_HCD_IRQHandler(&hhcd);
  return (0);
}

/**
  * @brief  This function handles USB-On-The-Go HS global interrupt request.
  * @param  None
  * @retval None
  */
u32 OTG_HS_IRQHandler(u32 Data)
{
  HAL_HCD_IRQHandler(&hhcd);
  return (0);
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
