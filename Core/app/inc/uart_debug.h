#ifndef _UART_DEBUG_H_
#define _UART_DEBUG_H_

#include "stm32f1xx_hal.h"      

// #define DEV_UART_DEBUG       //define this to start uart message output

/*format*/
// #ifdef DEV_UART_DEBUG
//   int run_count = 0;
// #endif

//   #ifdef DEV_UART_DEBUG
//     UART_Printf("voltage_pid:%d\r\n",run_count);
//   #endif

int UART_Printf(const char*fmt, ...);

#endif // !_UART_DEBUG_H_