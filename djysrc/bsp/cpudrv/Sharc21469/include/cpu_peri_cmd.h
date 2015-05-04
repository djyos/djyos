//----------------------------------------------------
// Copyright (c) 2014, SHENZHEN PENGRUI SOFT CO LTD. All rights reserved.

// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:

// 1. Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice, 
//    this list of conditions and the following disclaimer in the documentation 
//    and/or other materials provided with the distribution.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------
// Copyright (c) 2014 著作权由深圳鹏瑞软件有限公司所有。著作权人保留一切权利。
// 
// 这份授权条款，在使用者符合下列条件的情形下，授予使用者使用及再散播本
// 软件包装原始码及二进位可执行形式的权利，无论此包装是否经改作皆然：
// 
// 1. 对于本软件源代码的再散播，必须保留上述的版权宣告、本条件列表，以
//    及下述的免责声明。
// 2. 对于本套件二进位可执行形式的再散播，必须连带以文件以及／或者其他附
//    于散播包装中的媒介方式，重制上述之版权宣告、本条件列表，以及下述
//    的免责声明。

// 免责声明：本软件是本软件版权持有人以及贡献者以现状（"as is"）提供，
// 本软件包装不负任何明示或默示之担保责任，包括但不限于就适售性以及特定目
// 的的适用性为默示性担保。版权持有人及本软件之贡献者，无论任何条件、
// 无论成因或任何责任主义、无论此责任为因合约关系、无过失责任主义或因非违
// 约之侵权（包括过失或其他原因等）而起，对于任何因使用本软件包装所产生的
// 任何直接性、间接性、偶发性、特殊性、惩罚性或任何结果的损害（包括但不限
// 于替代商品或劳务之购用、使用损失、资料损失、利益损失、业务中断等等），
// 不负任何责任，即在该种使用已获事前告知可能会造成此类损害的情形下亦然。
//-----------------------------------------------------------------------------


#ifndef __CPU_PERI_CMD_H__
#define __CPU_PERI_CMD_H__
#ifdef __cplusplus
extern "C" {
#endif

#define     cn_uart_connect_recv_evtt    0
#define     cn_uart_disconnect_recv_evtt 1

u32 cmd_int(ufast_t uart_int_line);
ptu32_t module_init_uart0(ptu32_t para);
ptu32_t cmd_hard_write(struct  tagPanDevice *cmd_dev,ptu32_t buf,
                            ptu32_t res2,ptu32_t len);
ptu32_t cmd_app_write(struct  tagPanDevice *cmd_dev,ptu32_t src_buf,
                            ptu32_t res,ptu32_t len);
ptu32_t cmd_app_read(struct  tagPanDevice *uart_dev,ptu32_t res,
                                        ptu32_t dst_buf,ptu32_t len);
ptu32_t cmd_ctrl(struct tagPanDevice *uart_dev,u32 cmd,
                   u32 data1,u32 data2);
/**
 * @brief Disables interrupts specified by the Mask parameter.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Interrupt mask. Use constants from group "Interrupt masks". This
 *        parameter is 8 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: UART0_C2, UART0_C3,
 *          UART1_C2, UART1_C3, UART2_C2, UART2_C3, UART3_C2, UART3_C3, UART4_C2,
 *          UART4_C3, UART5_C2, UART5_C3 (depending on the peripheral).
 * @par Example:
 *      @code
 *      UART_PDD_DisableInterrupt(deviceID, UART_PDD_INTERRUPT_TRANSMITTER);
 *      @endcode
 */
#define UART_PDD_DisableInterrupt(PeripheralBase, Mask) ( \
    (UART_C2_REG(PeripheralBase) &= \
     (uint8_t)(~(uint8_t)((uint8_t)(Mask) & (uint8_t)(~(uint8_t)0xFU)))), \
    (UART_C3_REG(PeripheralBase) &= \
     (uint8_t)(~(uint8_t)((uint8_t)(Mask) & 0xFU))) \
  )

/**
 * @brief Enables interrupts specified by the Mask parameter.
 * @param PeripheralBase Peripheral base address.
 * @param Mask Interrupt mask. Use constants from group "Interrupt masks". This
 *        parameter is 8 bits wide.
 * @return Returns a value of void type.
 * @remarks The macro accesses the following registers: UART0_C2, UART0_C3,
 *          UART1_C2, UART1_C3, UART2_C2, UART2_C3, UART3_C2, UART3_C3, UART4_C2,
 *          UART4_C3, UART5_C2, UART5_C3 (depending on the peripheral).
 * @par Example:
 *      @code
 *      UART_PDD_EnableInterrupt(deviceID, UART_PDD_INTERRUPT_TRANSMITTER);
 *      @endcode
 */
#define UART_PDD_EnableInterrupt(PeripheralBase, Mask) ( \
    (UART_C2_REG(PeripheralBase) |= \
     (uint8_t)((uint8_t)(Mask) & (uint8_t)(~(uint8_t)0xFU))), \
    (UART_C3_REG(PeripheralBase) |= \
     (uint8_t)((uint8_t)(Mask) & 0xFU)) \
  )
#define PDD_DISABLE 0u
#define PDD_ENABLE  1u
#define UART_PDD_INTERRUPT_RECEIVER  (0x20u)
#define UART_PDD_INTERRUPT_TRANSMITTER          (UART_C2_TIE_MASK)
#ifdef __cplusplus
}
#endif
#endif
