/*23 bytes for 1 tick(1ms) timeout. if command max size is 200 bytes, then need
  at least 9 ms*/
#define UART_TX_TIMEOUT (9)