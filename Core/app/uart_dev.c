#include "uart_dev.h"   

#include "usart.h"
//lock
__weak int  UART_lock_simple(void)   { return 0;}
__weak void UART_unlock_simple(void) { }

dev_uart_status_t dev_UART_transmit(dev_uart_t dev_uart, const uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    HAL_StatusTypeDef res;
    UART_HandleTypeDef *uart;
    if(dev_uart == DEV_UART2)
    {
        uart = &huart2;
    }
    if(UART_lock_simple() == 0)
    {
        res = HAL_UART_Transmit(uart,pData,Size,Timeout);
        UART_unlock_simple();
        switch (res)
        {
            case HAL_OK:        return DEV_UART_OK;
            case HAL_ERROR:     return DEV_UART_ERR_UART;
            case HAL_BUSY:      return DEV_UART_ERR_BUSY;
            case HAL_TIMEOUT:   return DEV_UART_ERR_BUSY;
            default:
                return DEV_UART_ERR_UART;
        }
    }else{
        return DEV_UART_ERR_BUSY;       //lock fail suppose busy
    }
}

