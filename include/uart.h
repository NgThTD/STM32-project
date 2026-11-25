#ifndef UART_H
#define UART_H
#include <stdint.h>

void uart_init(uint32_t pclk_hz, uint32_t baud);
void uart_putc(char c);
void uart_puts(const char *s);
void uart_put_hex(uint32_t v);
void uart_put_dec(uint32_t v);
int  uart_getc_nb(void);   /* tra ve -1 neu chua co du lieu */

#endif
