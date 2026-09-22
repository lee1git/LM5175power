#ifndef _UART_DEV_H_
#define _UART_DEV_H_

#include "stm32f1xx_hal.h"      

// define zone
#define DEV_UART_TRANS_TIMEOUT      (5U)       //5ms

// enum zone
typedef enum{
    DEV_UART1 = 0,
    DEV_UART2,
    DEV_UART3,
    DEV_UARTMAX
} dev_uart_t;

typedef enum{
    DEV_UART_OK = 0,
    DEV_UART_ERR_BUSY,                //timeout or block
    DEV_UART_ERR_UART,
    DEV_UART_ERR_INVALID_DATA,        //invalid data
    DEV_UART_ERR_INVALID_INPUT,       //invalid input
    DEV_UART_ERR_UART_NOT_IMPLEMENTED  //uart not implemented for this platform
} dev_uart_status_t;

//functions
int UART_lock_simple(void);
void UART_unlock_simple(void);

dev_uart_status_t dev_UART_transmit(dev_uart_t dev_uart, const uint8_t *pData, uint16_t Size, uint32_t Timeout);


#endif // !_UART_DEV_H_