#include "uart.h"

int main(void)
{
    uart_init(64000000u, 115200u);   /* HSI 64 MHz sau reset */
    uart_puts("Hello STM32H750\n");

    while (1) {
        int c = uart_getc_nb();
        if (c >= 0) uart_putc((char)c);   /* echo lai ky tu nhan duoc */
    }
}
