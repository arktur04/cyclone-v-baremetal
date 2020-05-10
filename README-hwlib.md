The original **hwlib** variant contains a mistake in the file **alt_generalpurpose_io.c** in the function **alt_gpio_port_int_status_clear**.

In the original variant:
```
ALT_STATUS_CODE alt_gpio_port_int_status_clear(ALT_GPIO_PORT_t gpio_pid,
        uint32_t clrmask)
{
    volatile uint32_t   *addr;

    if (clrmask & ~ALT_GPIO_BITMASK)      { return ALT_E_ERROR; }
    if (gpio_pid == ALT_GPIO_PORTA)      { addr = ALT_GPIO0_INTSTAT_ADDR; }
    else if (gpio_pid == ALT_GPIO_PORTB) { addr = ALT_GPIO1_INTSTAT_ADDR; }
    else if (gpio_pid == ALT_GPIO_PORTC) { addr = ALT_GPIO2_INTSTAT_ADDR; }
    else { return ALT_E_BAD_ARG; }         /* argument error */

    alt_write_word(addr, clrmask);
    return ALT_E_SUCCESS;
}
```

Must be:
```
uint32_t alt_gpio_port_int_status_get(ALT_GPIO_PORT_t gpio_pid)
{
    volatile uint32_t   *addr;

    if (gpio_pid == ALT_GPIO_PORTA)      { addr = ALT_GPIO0_INTSTAT_ADDR; }
    else if (gpio_pid == ALT_GPIO_PORTB) { addr = ALT_GPIO1_INTSTAT_ADDR; }
    else if (gpio_pid == ALT_GPIO_PORTC) { addr = ALT_GPIO2_INTSTAT_ADDR; }
    else { return 0; }         /* error */

    return alt_read_word(addr);
}
```
