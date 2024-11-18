#include <stdio.h>
#include <string.h>
/*stm32 header*/
#include "stm32f4xx_hal.h"

/*rtos header*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "FreeRTOSConfig.h"

/*user header*/
#include "do_printf.h"
#include "do_scanf.h"
#include "cmd_shell.h"
#include "platform.h"
#include "util.h"

extern UART_HandleTypeDef huart2;
char cmd_buffer[128];
QueueHandle_t cmd_queue = NULL;

HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout);

static uint8_t cmd_help(const char *cmd_input);
static uint8_t cmd_memory_read(const char *cmd_input);
static uint8_t cmd_memory_write(const char *cmd_input);

static cmd_input_t g_cmd_table[] =
{
    {"help", "help : view a list of available commands\r\n", cmd_help, 0},
    {"memrl", "memrl : memrl <address>\r\n", cmd_memory_read, 1},
    {"memwl", "memwl : memwl <address> <value>\r\n", cmd_memory_write, 2}
};

static uint8_t cmd_help(const char *cmd_input)
{
    uint8_t i = 0, cmd_table_size = sizeof(g_cmd_table)/sizeof(g_cmd_table[0]);
    for(;i < cmd_table_size; i++)
    {
        simple_printf("%s", g_cmd_table[i].help);
    }
    return 0;
}

static uint8_t cmd_memory_read(const char *cmd_input)
{
    char input[32];
    uint32_t addr, value;
    simple_sscanf(cmd_input, "%s %x", input, &addr);
    value = do_memory_read(addr);
    simple_printf("0x%x", value);
    return pdTRUE;
}

static uint8_t cmd_memory_write(const char *cmd_input)
{
    char input[32];
    uint32_t addr, value;
    simple_sscanf(cmd_input, "%s %x %x", input, &addr, &value);
    do_memory_write(addr, value);
    return pdTRUE;
}

char *g_common_str[] = {
    "Incorrect command parameter(s).  Enter \"help\" to view a list of available commands.",
};

static uint8_t get_parameter_num(const char *cmd_input)
{
    uint8_t is_space = 0, cnt = 0;
    while(*cmd_input)
    {
        /*abcd   ffdd*/
        if(*cmd_input == ' ')
        {
            if(is_space == 0)
            {
                cnt++;
                is_space = 1;
            }
        }
        else
        {
            is_space = 0;
        }
        cmd_input++;
    }
    return cnt;
}

static void cmd_output(char *cmd_str, uint8_t cmd_str_len)
{
    if(cmd_str_len == 1)
    {
        switch(*cmd_str)
        {
            case '\b':
            {
                HAL_UART_Transmit(&huart2, (uint8_t *)"\b ", 2, UART_TX_TIMEOUT);
                break;
            }
            default:
                break;
        }
    }
    HAL_UART_Transmit(&huart2, (uint8_t *)cmd_str, cmd_str_len, UART_TX_TIMEOUT);
}

static void cmd_task(void *p)
{
    cmd_queue = xQueueCreate(128, sizeof(char));
    char received_char;
    uint8_t received_str_len = 0, cmd_table_size = sizeof(g_cmd_table) / sizeof(g_cmd_table[0]);
    uint8_t is_cmd_match = 0;
    
    for(;;)
    {
        uint8_t i = 0;
        if(pdTRUE == xQueueReceive(cmd_queue, (void *)&received_char, portMAX_DELAY))
        {
            if(received_char != '\r')
            {
                cmd_buffer[received_str_len++] = received_char;
                cmd_output(&received_char, sizeof(received_char));
                continue;
            }
            else
            {
                /*press ENTER*/
                cmd_output("\r\n", strlen("\r\n"));
                received_str_len = 0;
                for(; i < cmd_table_size; i++)
                {
                    if(strncmp(cmd_buffer, g_cmd_table[i].name, strlen(g_cmd_table[i].name)) == 0)
                    {
                        if(get_parameter_num(cmd_buffer) == g_cmd_table[i].parameter)
                        {
                            is_cmd_match = 1;
                        }
                        else
                        {
                            is_cmd_match = 0;
                        }
                        break;
                    }
                    else
                    {
                        is_cmd_match = 0;
                    }
                }
                if(is_cmd_match)
                {
                    g_cmd_table[i].callback(cmd_buffer);
                }
                else
                {
                    cmd_output(g_common_str[0], strlen(g_common_str[0]));
                }
                cmd_output("\r\n#", strlen("\r\n#"));
                memset(cmd_buffer, 0, sizeof(cmd_buffer));
            }
        }
    }    
}


void cmd_init(void)
{
    xTaskCreate((TaskFunction_t)cmd_task, "cmd", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES-1, NULL);
}