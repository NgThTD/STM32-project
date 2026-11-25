#include "uart.h"

#define REG(a)  (*(volatile uint32_t *)(a))

#define RCC_BASE      0x58024400u
#define RCC_AHB4ENR   REG(RCC_BASE + 0xE0)
#define RCC_APB2ENR   REG(RCC_BASE + 0xF0)

#define GPIOB_BASE    0x58020400u
#define GPIOB_MODER   REG(GPIOB_BASE + 0x00)
#define GPIOB_AFRH    REG(GPIOB_BASE + 0x24)

#define USART1_BASE   0x40011000u
#define USART1_CR1    REG(USART1_BASE + 0x00)
#define USART1_BRR    REG(USART1_BASE + 0x0C)
#define USART1_ISR    REG(USART1_BASE + 0x1C)
#define USART1_RDR    REG(USART1_BASE + 0x24)
#define USART1_TDR    REG(USART1_BASE + 0x28)

#define ISR_RXNE (1u << 5)
#define ISR_TC   (1u << 6)
#define ISR_TXE  (1u << 7)

void uart_init(uint32_t pclk_hz, uint32_t baud)
{
    RCC_AHB4ENR |= (1u << 1);              /* GPIOB clock */
    RCC_APB2ENR |= (1u << 4);              /* USART1 clock */
    (void)RCC_APB2ENR;                     /* doc lai de cho clock on dinh */

    /* PB14, PB15 -> Alternate function (0b10) */
    GPIOB_MODER &= ~((3u << 28) | (3u << 30));
    GPIOB_MODER |=  ((2u << 28) | (2u << 30));
    /* AFRH: PB14 -> bit 24..27, PB15 -> bit 28..31, AF4 */
    GPIOB_AFRH  &= ~((0xFu << 24) | (0xFu << 28));
    GPIOB_AFRH  |=  ((4u << 24) | (4u << 28));

    USART1_CR1 = 0;                        /* tat truoc khi cau hinh */
    USART1_BRR = (pclk_hz + baud / 2) / baud;
    USART1_CR1 = (1u << 3) | (1u << 2) | 1u;   /* TE | RE | UE */
}

void uart_putc(char c)
{
    while (!(USART1_ISR & ISR_TXE)) {}
    USART1_TDR = (uint32_t)c;
}

void uart_puts(const char *s)
{
    while (*s) {
        if (*s == '\n') uart_putc('\r');
        uart_putc(*s++);
    }
    while (!(USART1_ISR & ISR_TC)) {}
}

void uart_put_hex(uint32_t v)
{
    static const char h[] = "0123456789ABCDEF";
    uart_putc('0'); uart_putc('x');
    for (int i = 28; i >= 0; i -= 4) uart_putc(h[(v >> i) & 0xF]);
}

void uart_put_dec(uint32_t v)
{
    char b[11];
    int i = 0;
    if (v == 0) { uart_putc('0'); return; }
    while (v) { b[i++] = (char)('0' + (v % 10)); v /= 10; }
    while (i) uart_putc(b[--i]);
}

int uart_getc_nb(void)
{
    return (USART1_ISR & ISR_RXNE) ? (int)(USART1_RDR & 0xFF) : -1;
}
