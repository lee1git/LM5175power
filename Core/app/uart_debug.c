#include "uart_debug.h"
#include "uart_dev.h"

#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "stddef.h"

int UART_Printf(const char*fmt, ...)
{
    char buf[128];
    int res;
    va_list args;
    va_start(args, fmt);
    res = vsnprintf(buf,sizeof(buf),fmt,args);
    va_end(args);

    if(dev_UART_transmit(DEV_UART2,(uint8_t *)buf,strlen(buf),20) != DEV_UART_OK)
    {
        return -1;
    }
    return res;
}