#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "stm32f4xx_hal.h"
#include "platform.h"

extern UART_HandleTypeDef huart2;

/*unsigned int to string*/
uint8_t simple_itoa(uint32_t i, uint8_t *buf, int carry)
{
    uint32_t j = 0;
    uint32_t k;
    do{
        k = i % carry; 
        if(k <= 9)
            buf[j++] = k + '0';
        else
            buf[j++] = k + '0' + 39;
        i = i / carry;
    }while(i);
    
    buf[j] = '\0';

    for(k=0; k<j/2; k++)
    {
        char tmp = buf[k];
        buf[k] = buf[j-1-k];
        buf[j-1-k] = tmp;
    }

    return j;
}

int simple_printf(const char *fmt, ...)
{
    va_list ap;
    static uint8_t str[128], size;
    const char *p;

    va_start(ap, fmt);
    for(p=fmt; *p; p++)
    {
        if(*p == '%')
        {
            char pat = *++p;
            switch(pat)
            {
                case 'd':
                {
                    size = simple_itoa(va_arg(ap, uint32_t), str, 10);
                    break;
                }
                case 'x':
                {
                    size = simple_itoa(va_arg(ap, uint32_t), str, 16);
                    break;
                }
                case 's':
                {
                    char *str_arg = va_arg(ap, char *);
                    size = strlen(str_arg);
                    for(int i=0; i<size; i++)
                    {
                        str[i] = str_arg[i];
                    }
                    break;
                }
                default:
                    break;
            }
            HAL_UART_Transmit(&huart2, str, size, UART_TX_TIMEOUT);
        }
        else if(*p == '\n')
        {
            str[0] = '\r';
            str[1] = '\n';
            HAL_UART_Transmit(&huart2, str, 2, UART_TX_TIMEOUT);
        }  
        else
        {
            HAL_UART_Transmit(&huart2, (uint8_t *)p, 1, UART_TX_TIMEOUT);
        }
    }
    va_end(ap);
    return 0;
}