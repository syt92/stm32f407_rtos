#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "stm32f4xx_hal.h"

#include "string.h"
#include "FreeRTOSConfig.h"

extern UART_HandleTypeDef huart2;
uint8_t cmd_buffer[128];
QueueHandle_t cmd_queue = NULL;

HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout);

/*23 bytes for 1 tick(1ms) timeout. if command max size is 200 bytes, then need
  at least 9 ms*/
#define UART_TX_TIMEOUT (9)
static void cmd_task(void *p)
{
    cmd_queue = xQueueCreate(128, sizeof(char));
    uint8_t uart_transmit_buf;
    for(;;)
    {
        if(pdTRUE == xQueueReceive(cmd_queue, (void *)&uart_transmit_buf, portMAX_DELAY))
        {
            HAL_UART_Transmit(&huart2, &uart_transmit_buf, 1, 1);
            HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
        }
    }    
}

void cmd_init(void)
{
    xTaskCreate((TaskFunction_t)cmd_task, "cmd", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES-1, NULL);
}