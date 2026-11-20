#include <stdint.h>

extern uint32_t _estack, _sidata, _sdata, _edata, _sbss, _ebss;
extern int main(void);

void Reset_Handler(void)
{
    /* Bat FPU (CP10, CP11 full access) */
    *(volatile uint32_t *)0xE000ED88 |= (0xFu << 20);
    __asm volatile ("dsb\n isb");

    /* Copy .data tu Flash sang RAM */
    uint32_t *src = &_sidata;
    uint32_t *dst = &_sdata;
    while (dst < &_edata) *dst++ = *src++;

    /* Xoa .bss */
    for (dst = &_sbss; dst < &_ebss; ) *dst++ = 0;

    main();
    while (1) {}
}

void Default_Handler(void)
{
    while (1) {}
}

__attribute__((section(".isr_vector"), used))
const uint32_t vectors[] = {
    (uint32_t)&_estack,           /* Initial MSP */
    (uint32_t)Reset_Handler,      /* Reset */
    (uint32_t)Default_Handler,    /* NMI */
    (uint32_t)Default_Handler     /* HardFault */
};