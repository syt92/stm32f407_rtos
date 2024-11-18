typedef uint8_t (*cmd_callback)(const char *);

typedef struct cmd_input
{
	const char * const name;
	const char * const help;
	const cmd_callback callback;
	uint8_t parameter;
}cmd_input_t;

void cmd_init(void);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);